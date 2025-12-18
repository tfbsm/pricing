#include <libtfbsm/core/pricing_engine.hpp>

#include "spdlog/spdlog.h"

void tfbsm::PricingEngine::onTick(tfbsm::Tick const& tick) {
    spdlog::debug("Pricing engine tick: {} {} b: {} a: {}", tick.ts, tick.symbol, tick.bid, tick.ask);

    parameter_estimator_->onTick(tick);

    if (tick.market == tfbsm::Tick::Market::kSpot) {
        spot_prices_.insert_or_assign(tick.symbol, (tick.bid + tick.ask) / 2);
        
        return;
    }

    // Do estimate price only on option ticks

    auto params = parameter_estimator_->get_parameters();
    
    // Do not process tick if there is no parameters estimated yet
    if (!params.has_value()) return;

    // TODO: estimate price here

    onEstimate(tick.ts, tick.symbol, (tick.bid + tick.ask) / 2);
}

void tfbsm::PricingEngine::onTicks(std::vector<tfbsm::Tick> const& ticks) {
    spdlog::debug("Pricing engine ticks: {}", ticks.size());

    parameter_estimator_->onTicks(ticks);

    for (auto const& tick : ticks) {
        if (tick.market == tfbsm::Tick::Market::kSpot) {
            spot_prices_.insert_or_assign(tick.symbol, (tick.bid + tick.ask) / 2);
        }
    }

    auto params = parameter_estimator_->get_parameters();
    
    // Do not process tick if there is no parameters estimated yet
    if (!params.has_value()) return;

    // FIXME: In fact may not be latest
    auto lastTick = ticks.back();

    if (lastTick.market == tfbsm::Tick::Market::kSpot) 
        return;
    
    // Estimate only on option ticks

    // TODO: estimate price here

    onEstimate(lastTick.ts, lastTick.symbol, (lastTick.bid + lastTick.ask) / 2);
}

void tfbsm::PricingEngine::onKlines(std::vector<tfbsm::OHLC> const& klines) {
    spdlog::debug("Pricing engine klines: {}", klines.size());

    parameter_estimator_->onKlines(klines);
}

void tfbsm::PricingEngine::onEstimate(std::time_t ts,
                                      std::string const& symbol, 
                                      double price) {
    spdlog::debug("Sending estimation: ts: {} symbol: {} price: {}", ts, symbol, price);

    auto option_info = tfbsm::ConfigurationRepository::getInstance().get_option(symbol);
    
    if (!option_info.has_value()) {
        spdlog::warn("Option info is unknown for symbol: {}", symbol);
        return;
    }

    auto spot_price = spot_prices_.find(option_info.value()->spot_symbol);

    if (spot_price == spot_prices_.end()) {
        spdlog::debug("Spot price for {} is not yet known", option_info.value()->spot_symbol);
        return;
    }

    auto spot_code = tfbsm::ConfigurationRepository::getInstance().get_symbol_code(option_info.value()->spot_symbol);
    auto option_code = tfbsm::ConfigurationRepository::getInstance().get_symbol_code(symbol);

    if (!spot_code.has_value() || !option_code.has_value())
        throw new std::runtime_error("spot or option code is unknown");

    tfbsm::PriceEstimation est{
        /* ts: */ ts,
        /* spot_symbol_code: */ spot_code.value(),
        /* option_symbol_code: */ option_code.value(),
        /* market_price: */ spot_price->second,
        /* estimated_price */ price, // FIXME!
    };

    price_sink_->onPriceEstimation(est);
}
