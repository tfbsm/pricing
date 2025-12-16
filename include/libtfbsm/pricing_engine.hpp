#ifndef TFBSM_PRICING_ENGINE_H_
#define TFBSM_PRICING_ENGINE_H_

#include "models.hpp"
#include "parameter_estimator.hpp"
#include "price_sink.hpp"
#include "stable_distribution.hpp"
#include "configuration_repository.hpp"

namespace tfbsm {

// Time-Fractional Black-Scholes model pricing engine
class PricingEngine : TickObserver, KlineObserver {
   public:
    PricingEngine(std::shared_ptr<PriceSink> priceSink,
                  std::unique_ptr<ParameterEstimator> parameterEstimator) noexcept 
                  : parameterEstimator(std::move(parameterEstimator)), priceSink(std::move(priceSink)) {
        distribution = StableDistribution(parameterEstimator->get_parameters().alpha,
                       std::pow(tfbsm::ConfigurationRepository::getInstance().get_dtau(), 
                                                    1. / parameterEstimator->get_parameters().alpha));
    };

    ~PricingEngine() = default;
    PricingEngine(PricingEngine const&) = default;
    PricingEngine& operator=(PricingEngine const&) = default;
    PricingEngine(PricingEngine&&) = default;
    PricingEngine& operator=(PricingEngine&&) = default;

    void onTick(Tick tick) override;                        // ???
    void onTicks(std::vector<Tick> const& ticks) override;  // ???

    void onKline(OHLC kline);
    void onKlines(std::vector<OHLC> const& klines) override;

    [[nodiscard]] double estimateFairPrice();

   private:
    StableDistribution distribution;
    std::shared_ptr<PriceSink> priceSink;
    std::unique_ptr<ParameterEstimator> parameterEstimator;

    /**
     * Simulates subordinator to the first crossing of level T
     *
     * @param T Time moment.
     * @return Double $\tau$ -- first crossing of level T.
     */
    double sampleFirstCrossing(double T);
};

}  // namespace tfbsm

#endif  // TFBSM_PRICING_ENGINE_H_
