#include "libtfbsm/zmq_price_source.hpp"

#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

void tfbsm::ZeroMQPriceSource::run() {
    zmq::message_t msg;

    std::cerr << "ZeroMQ price source running" << std::endl;

    while (true) {
        auto res = sock_.recv(msg, zmq::recv_flags::dontwait);
        if (!res.has_value())
            continue;
        
        std::string msg_str = std::string(static_cast<char*>(msg.data()), msg.size());

        std::cout << "Recv message: " << msg_str << std::endl;

        tfbsm::Tick tick;

        try {
            json data = json::parse(msg_str);

            tick.ts = static_cast<int64_t>(data["ts"].get<double>());
            tick.bid = data["bid"].get<double>();
            tick.ask = data["ask"].get<double>();
            tick.symbol = data["symbol"].get<std::string>();
            tick.market = static_cast<tfbsm::Tick::Market>(data["market"].get<int>());
        } catch (json::exception const& err) {
            std::cerr << "message parse error " << err.what() << std::endl;
            continue;
        }

        std::cout << "recv tick: " << tick.ts << " bid: " << tick.bid << " ask: " << tick.ask << std::endl;

        {
            std::shared_lock<std::shared_mutex> lock(mtx_);

            for (auto &tick_observer : tick_oververs_) 
                tick_observer->onTick(tick);
        }
    }
}
