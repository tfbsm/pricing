#ifndef TFBSM_PRICING_ENGINE_H_
#define TFBSM_PRICING_ENGINE_H_

#include "parameter_estimator.hpp"
#include "price_sink.hpp"
#include "stable_distribution.hpp"

namespace tfbsm {

struct OHLC {
    time_t T;
    double Open;
    double High;
    double Low;
    double Close;
};

// Time-Fractional Black-Scholes model pricing engine
class PricingEngine {
   public:
    PricingEngine(double alpha, double dtau, PriceSink&& priceSink,
                  ParameterEstimator&& parameterEstimator) noexcept
        : distribution(alpha, std::pow(dtau, 1. / alpha)),
          priceSink(std::move(priceSink)),
          parameterEstimator(std::move(parameterEstimator)) {};

    ~PricingEngine() = default;
    PricingEngine(PricingEngine const&) = default;
    PricingEngine& operator=(PricingEngine const&) = default;
    PricingEngine(PricingEngine&&) = default;
    PricingEngine& operator=(PricingEngine&&) = default;

    void setAlpha(double alpha);

    void onTick(OHLC tick);                        // ???
    void onTicks(std::vector<OHLC> const& ticks);  // ???

    [[nodiscard]] double estimateFairPrice(double T);

   private:
    StableDistribution distribution;
    PriceSink priceSink;
    ParameterEstimator parameterEstimator;

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
