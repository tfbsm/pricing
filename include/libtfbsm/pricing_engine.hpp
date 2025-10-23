#ifndef TFBSM_PRICING_ENGINE_H_
#define TFBSM_PRICING_ENGINE_H_

#include "stable_distribution.hpp"

namespace tfbsm {

class PricingEngine {
   public:
    PricingEngine(double alpha,
                  double dtau,
                  double T) noexcept
        : distribution(alpha, std::pow(dtau, 1. / alpha)) {};

    ~PricingEngine() = default;
    PricingEngine(PricingEngine const&) = default;
    PricingEngine& operator=(PricingEngine const&) = default;
    PricingEngine(PricingEngine&&) = default;
    PricingEngine& operator=(PricingEngine&&) = default;

    void calculate();

   private:
    StableDistribution distribution;
};

}  // namespace tfbsm

#endif  // TFBSM_PRICING_ENGINE_H_