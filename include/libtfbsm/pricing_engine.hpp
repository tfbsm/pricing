#ifndef TFBSM_PRICING_ENGINE_H_
#define TFBSM_PRICING_ENGINE_H_

#include "stable_distribution.hpp"

namespace tfbsm {

// Time-Fractional Black-Scholes model pricing engine
class PricingEngine {
   public:
    PricingEngine(double alpha, double dtau) noexcept
        : distribution(alpha, std::pow(dtau, 1. / alpha)) {};

    ~PricingEngine() = default;
    PricingEngine(PricingEngine const&) = default;
    PricingEngine& operator=(PricingEngine const&) = default;
    PricingEngine(PricingEngine&&) = default;
    PricingEngine& operator=(PricingEngine&&) = default;

    void setAlpha(double alpha);

    [[nodiscard]] double estimateFairPrice(double T);

   private:
    StableDistribution distribution;

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
