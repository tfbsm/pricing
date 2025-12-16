#include "libtfbsm/zmq_price_sink.hpp"

#include <cstdint>
#include <iostream>
#include "spdlog/spdlog.h"

#include "libtfbsm/protocol.hpp"

void tfbsm::ZeroMQPriceSink::run() {
    spdlog::info("Price sink listening on {}", bind_address_);

    sock_.bind(bind_address_);
}

void tfbsm::ZeroMQPriceSink::onPriceEstimation(tfbsm::PriceEstimation const& estimation) {
    tfbsm::protocol::Messages::PriceEstimation estimationPacket{
        estimation.option_symbol_code,
        estimation.spot_symbol_code,
        precision_,
        static_cast<uint64_t>(estimation.market_price * pow(10, precision_)),
        static_cast<uint64_t>(estimation.estimated_price * pow(10, precision_)),
        static_cast<uint64_t>(estimation.ts),
        0,
    };

    auto res = tfbsm::protocol::Messages::serialize(estimationPacket);

    // std::cout << "sending packet ";

    // for (auto const b : res) {
    //     std::cout << std::hex << std::setfill('0') << std::setw(2) << (int)b;
    // }

    // std::cout << std::endl;

    zmq::message_t msg(res);
    sock_.send(msg, zmq::send_flags::dontwait);
}
