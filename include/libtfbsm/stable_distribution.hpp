#ifndef TFBSM_STABLEDISTRIBUTION_H_
#define TFBSM_STABLEDISTRIBUTION_H_

#include <cmath>
#include <cstdint>
#include <random>
#include <vector>

namespace tfbsm {

class StableDistribution {
   public:
    StableDistribution(double alpha = .5, double gamma = 1) noexcept
        : alpha_(alpha), gamma_(gamma) {};

    ~StableDistribution() = default;
    StableDistribution(StableDistribution const&) = default;
    StableDistribution& operator=(StableDistribution const&) = default;
    StableDistribution(StableDistribution&&) noexcept = default;
    StableDistribution& operator=(StableDistribution&&) noexcept = default;

    inline double sampleValue() noexcept;
    void sampleValues(std::vector<double>& results) noexcept;
    std::vector<double> sampleValues(std::size_t n) noexcept;

   private:
    double alpha_{.5};
    double gamma_{1};

    std::uniform_real_distribution<> uniform_{-M_PI_2, M_PI_2};
    std::exponential_distribution<> exp_{1};
};

}  // namespace tfbsm

#endif  // TFBSM_STABLEDISTRIBUTION_H_