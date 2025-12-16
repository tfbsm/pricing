#include <chrono>
#include <iostream>

#include "libtfbsm/models.hpp"
#include "libtfbsm/zmq_price_sink.hpp"
#include "libtfbsm/zmq_price_source.hpp"

typedef std::chrono::steady_clock::time_point tp;
typedef std::chrono::steady_clock clock_;

int main(int argc, char const* argv[]) {
    zmq::context_t ctx;
    tfbsm::ZeroMQPriceSource source("tcp://127.0.0.1:5556", ctx);
    tfbsm::ZeroMQPriceSink sink("tcp://*:5555", ctx, 6);

    sink.run();

    while (true) {
        tfbsm::PriceEstimation est{
            time(0), 1337, 1337, 1.337 * rand(), 31.337 * rand(),
        };

        sink.onPriceEstimation(est);
    }
}