#include <libtfbsm/core/pricing_engine.hpp>

#include "spdlog/spdlog.h"

// double tfbsm::PricingEngine estimateFairPrice()
void tfbsm::PricingEngine::onTick(tfbsm::Tick const& tick) {
    spdlog::debug("Pricing engine tick: {} {} b: {} a: {}", tick.ts, tick.symbol, tick.bid, tick.ask);

    parameter_estimator_->onTick(tick);

    parameter_estimator_->get_parameters();
}

void tfbsm::PricingEngine::onTicks(std::vector<tfbsm::Tick> const& ticks) {
    spdlog::debug("Pricing engine ticks: {}", ticks.size());

    parameter_estimator_->onTicks(ticks);
}

void tfbsm::PricingEngine::onKlines(std::vector<tfbsm::OHLC> const& klines) {
    spdlog::debug("Pricing engine klines: {}", klines.size());

    parameter_estimator_->onKlines(klines);
}
