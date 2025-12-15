#include "libtfbsm/zmq_price_sink.hpp"

#include "libtfbsm/feeder_protocol.hpp"

#include <iostream>
#include <cstdint>

void tfbsm::ZeroMQPriceSink::run() {
    sock_.bind(bind_address_);
}

void tfbsm::ZeroMQPriceSink::onPriceEstimation(const tfbsm::PriceEstimation& estimation) {
    tfbsm::Protocol::PriceEstimation estimationPacket{
        estimation.option_symbol_code,
        estimation.spot_symbol_code,
        precision_,
        static_cast<uint64_t>(estimation.market_price*pow(10, precision_)),
        static_cast<uint64_t>(estimation.estimated_price*pow(10, precision_)),
        static_cast<uint64_t>(estimation.ts),
        0,
    };

    auto res = tfbsm::Protocol::serialize(estimationPacket);

    std::cout << "\rsending packet ";
    
    for (const auto b : res) {
        std::cout << std::hex << std::setfill('0') << std::setw(2) << (int) b;
    }

    // std::cout << std::endl;

    zmq::message_t msg(res);
    sock_.send(msg, zmq::send_flags::dontwait);
}
