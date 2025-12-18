#include <libtfbsm/interactions/zmq_price_source.hpp>

#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

void tfbsm::ZeroMQPriceSource::run() {
    zmq::message_t msg;

    spdlog::info("ZeroMQ price source running");

    while (true) {
        auto res = sock_.recv(msg, zmq::recv_flags::dontwait);
        if (!res.has_value())
            continue;
        
        std::string msg_str = std::string(static_cast<char*>(msg.data()), msg.size());

        // spdlog::debug("Recv message: {}", msg_str);

        std::optional<tfbsm::Tick> tick;
        std::optional<tfbsm::OHLC> kline;

        try {
            json data = json::parse(msg_str);

            auto type = data["type"].get<std::string>();

            if (type == "tick") {
                tfbsm::Tick _tick;

                std::chrono::system_clock::time_point tp{std::chrono::milliseconds{data["ts"].get<int64_t>()}};

                _tick.ts = tp;
                _tick.bid = data["bid"].get<double>();
                _tick.ask = data["ask"].get<double>();
                _tick.symbol = data["symbol"].get<std::string>();
                _tick.market = static_cast<tfbsm::Tick::Market>(data["market"].get<int>());

                tick = std::move(_tick);
            } else if (type == "kline") {
                tfbsm::OHLC _kline;

                std::chrono::system_clock::time_point tp{std::chrono::milliseconds{data["ts"].get<int64_t>()}};

                _kline.ts = tp;
                _kline.symbol = data["symbol"].get<std::string>();
                _kline.open = data["open"].get<double>();
                _kline.high = data["high"].get<double>();
                _kline.low = data["low"].get<double>();
                _kline.close = data["close"].get<double>();
                _kline.volume = data["volume"].get<double>();
                _kline.timescale_s = data["timescale_s"].get<uint32_t>();

                kline = std::move(_kline);
            } else {
                spdlog::warn("Unknown message type: {}", type);
            }
        } catch (json::exception const& err) {
            spdlog::warn("Incoming message parse error: {}", err.what());
            continue;
        }

        {
            std::shared_lock<std::shared_mutex> lock(mtx_);

            if (tick.has_value())
                for (auto &tick_observer : tick_oververs_) 
                    tick_observer->onTick(tick.value());
            
            if (kline.has_value())
                for (auto &kline_observer : kline_observers_)
                    kline_observer->onKlines(std::vector<tfbsm::OHLC>{kline.value()});
        }
    }
}
