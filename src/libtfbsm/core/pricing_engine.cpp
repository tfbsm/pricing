#include <libtfbsm/core/pricing_engine.hpp>

#include "spdlog/spdlog.h"

/// Public API

void tfbsm::PricingEngine::onTick(tfbsm::Tick const& tick) {
    onTick(tick, false);
}

void tfbsm::PricingEngine::onTicks(std::vector<tfbsm::Tick> const& ticks) {
    // spdlog::debug("Pricing engine ticks: {}", ticks.size());

    parameter_estimator_->onTicks(ticks);

    for (auto const& tick : ticks) {
        onTick(tick, true);
    }
}

void tfbsm::PricingEngine::onKlines(std::vector<tfbsm::OHLC> const& klines) {
    spdlog::debug("[PricingEngine] Pricing engine klines: {}", klines.size());

    parameter_estimator_->onKlines(klines);
}


/// Private API

void tfbsm::PricingEngine::onTick(tfbsm::Tick const& tick, bool batch) {
    spdlog::debug("[PricingEngine] Pricing engine tick: {} {} b: {} a: {}", tick.ts.time_since_epoch().count(), tick.symbol, tick.bid, tick.ask);

    if (!batch) parameter_estimator_->onTick(tick);

    if (tick.market == tfbsm::Tick::Market::kSpot) {
        spot_prices_[tick.symbol] = (tick.bid + tick.ask) / 2.0;
        return;
    }

    // Do estimate price only on option ticks
    auto params = parameter_estimator_->get_parameters();
    
    // Do not process tick if there is no parameters estimated yet
    if (!params.has_value()) return;

    auto option_info = tfbsm::ConfigurationRepository::getInstance().get_option(tick.symbol);
    if (!option_info.has_value()) {
        spdlog::warn("[PricingEngine] Option info is unknown for symbol: {}", tick.symbol);
        return;
    }

    auto spot_price = spot_prices_.find(option_info.value()->spot_symbol);
    if (spot_price == spot_prices_.end()) {
        spdlog::debug("[PricingEngine] Spot price for {} is not yet known", option_info.value()->spot_symbol);
        return;
    }

    auto estimated_price = estimate(spot_price->second, option_info.value(), params.value());

    spdlog::debug("[PricingEngine] Estimated price {}", estimated_price);
    
    sendEstimation(tick.ts, tick.symbol, option_info.value(), spot_price->second, estimated_price);
}

double tfbsm::PricingEngine::estimate(
    double spot_price,
    std::shared_ptr<const tfbsm::Option> option,
    tfbsm::ParameterEstimator::Parameters const& parameters
) const {
    const int size = 50000;
    double r = tfbsm::ConfigurationRepository::getInstance().get_risk_free_rate();

    if (!(parameters.alpha > 0 && parameters.alpha <= 1)) {
        throw std::invalid_argument("Invalid alpha. Alpha must be in (0, 1).");
    }

    if (std::abs(parameters.alpha - 1.0) < 1e1) { //fixme
        return black_scholes_price(spot_price, parameters.sigma, option, r);
    }
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<double> normal(0.0, 1.0);
    
    tfbsm::StableDistribution subordinator(parameters.alpha);
    
    double sum = 0.0;
    double sumSquares = 0.0;
    double T = option->get_time_to_expiration();
    
    for (int i = 0; i < size; ++i) {
        double s1 = subordinator.sampleValue();
        
        double e_t = std::pow(T / std::abs(s1), parameters.alpha);

        // spdlog::debug("!!!!!!!!!!!!!!!!!!!!!!!! sampleValue {} {}", e_t, T);

        
        if (!std::isfinite(e_t) || e_t <= 0) {
            e_t = T;
        }
        
        double z_norm = normal(gen);
        
        double drift = (r - 0.5 * parameters.sigma * parameters.sigma) * e_t;
        double diffusion = parameters.sigma * std::sqrt(e_t) * z_norm;
        double S_T = spot_price * std::exp(drift + diffusion);
        
        double payoff;
        if (option->type == tfbsm::Option::OptionType::kCall) {
            payoff = std::max(S_T - option->strike_price, 0.0);
        } else {
            payoff = std::max(option->strike_price - S_T, 0.0);
        }
        
        // spdlog::debug("!!!!!!!!!!!!!!!!!!!!!!!! payoff {} {}", payoff, S_T);

        double discounted_payoff = std::exp(-r * e_t) * payoff;
        
        sum += discounted_payoff;
        sumSquares += discounted_payoff * discounted_payoff;
    }
    
    double mean = sum / size;

    // spdlog::debug("!!!!!!!!!!!!!!!!!!!!!!!! sampleValue {} {}", e_t, T);
    
    return mean;
}

double tfbsm::PricingEngine::black_scholes_price(
    double spot_price,
    double sigma,
    std::shared_ptr<const tfbsm::Option> option,
    double r
) const noexcept {
    auto T = option->get_time_to_expiration() / (60 * 60 * 24 * 365);

    spdlog::debug("BS T: {}", T);

    if (T <= 0) {
        return (option->type == tfbsm::Option::OptionType::kCall) 
                ? 
               std::max(spot_price - option->strike_price, 0.0) 
               :
               std::max(option->strike_price - spot_price, 0.0);
    }
    
    double d1 = (std::log(spot_price / option->strike_price) + (r + 0.5 * sigma * sigma) * T) / 
               (sigma * std::sqrt(T));
    double d2 = d1 - sigma * std::sqrt(T);
    
    auto norm_cdf = [](double x) {
        return 0.5 * (1.0 + std::erf(x / std::sqrt(2.0)));
    };
    
    if (option->type == tfbsm::Option::OptionType::kCall) {
        return spot_price * norm_cdf(d1) - option->strike_price * std::exp(-r * T) * norm_cdf(d2);
    } else {
        return option->strike_price * std::exp(-r * T) * norm_cdf(-d2) - spot_price * norm_cdf(-d1);
    }
}

void tfbsm::PricingEngine::sendEstimation(std::chrono::system_clock::time_point ts,
                                        std::string const& symbol, 
                                        std::shared_ptr<tfbsm::Option> option_info,
                                        double spot_price,
                                        double price) {
    spdlog::debug("[PricingEngine] Sending estimation: ts: {} symbol: {} price: {}", ts.time_since_epoch().count(), symbol, price);

    auto spot_code = tfbsm::ConfigurationRepository::getInstance().get_symbol_code(option_info->spot_symbol);
    auto option_code = tfbsm::ConfigurationRepository::getInstance().get_symbol_code(symbol);

    if (!spot_code.has_value() || !option_code.has_value())
        throw new std::runtime_error("spot or option code is unknown");

    tfbsm::PriceEstimation est{
        /* ts: */ ts,
        /* spot_symbol_code: */ spot_code.value(),
        /* option_symbol_code: */ option_code.value(),
        /* market_price: */ spot_price,
        /* estimated_price */ price, // FIXME!
    };

    price_sink_->onPriceEstimation(est);
}