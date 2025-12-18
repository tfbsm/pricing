#include <chrono>
#include <iostream>

#include <libtfbsm/core/models.hpp>
#include <libtfbsm/core/parameter_estimator.hpp>
#include <libtfbsm/core/pricing_engine.hpp>
#include <libtfbsm/interactions/zmq_price_sink.hpp>
#include <libtfbsm/interactions/zmq_price_source.hpp>

typedef std::chrono::steady_clock::time_point tp;
typedef std::chrono::steady_clock clock_;

int main(int argc, char const* argv[]) {
    spdlog::set_level(spdlog::level::debug);
    
    zmq::context_t ctx;

    auto sink = std::make_shared<tfbsm::ZeroMQPriceSink>("tcp://*:5555", ctx, 6);
    sink->run();

    tfbsm::PricingEngine pricing_engine(
        static_cast<std::shared_ptr<tfbsm::PriceSink>>(sink),
        std::make_unique<tfbsm::ParameterEstimator>()
    );

    tfbsm::ZeroMQPriceSource source("tcp://127.0.0.1:5556", ctx);
    source.addTickObserver(pricing_engine);
    source.addKlineObserver(pricing_engine);

    while (true) {
        tfbsm::PriceEstimation est{
            time(0), 1337, 1337, 1.337 * rand(), 31.337 * rand(),
        };

        sink->onPriceEstimation(est);
    }
}