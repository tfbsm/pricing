#include <chrono>
#include <fstream>
#include <iostream>

#include <libtfbsm/configuration_repository.hpp>
#include <libtfbsm/core/models.hpp>
#include <libtfbsm/core/parameter_estimator.hpp>
#include <libtfbsm/core/pricing_engine.hpp>
#include <libtfbsm/interactions/zmq_price_sink.hpp>
#include <libtfbsm/interactions/zmq_price_source.hpp>

typedef std::chrono::steady_clock::time_point tp;
typedef std::chrono::steady_clock clock_;

int main(int argc, char const* argv[]) {
    spdlog::set_level(spdlog::level::debug);

    if (argc < 2) {
        spdlog::error("Config file is required");
        return -1;
    }

    std::ifstream file(argv[1], std::ios::binary);
    if (!file.is_open()) {
        spdlog::error("Can't open config file {}", argv[1]);
        return -1;
    }

    nlohmann::json configJson;
    file >> configJson;

    tfbsm::ConfigurationRepository::getInstance().from_json(configJson);

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