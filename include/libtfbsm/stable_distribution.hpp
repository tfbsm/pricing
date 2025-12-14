#ifndef TFBSM_STABLEDISTRIBUTION_H_
#define TFBSM_STABLEDISTRIBUTION_H_

#include <random>
#include <vector>

namespace tfbsm {

// Stable distribution that is generated using
// Chambers Mallows Stuck (CMS) method.
class StableDistribution {
   public:
    /**
     * StableDistribution constructor
     *
     * @param alpha Stable distribution's alpha.
     * @param gamma Stable distribution's gamma.
     */
    StableDistribution(double alpha = .5, double gamma = 1) noexcept
        : alpha_(alpha), gamma_(gamma) {};

    ~StableDistribution() = default;
    StableDistribution(StableDistribution const&) = default;
    StableDistribution& operator=(StableDistribution const&) = default;
    StableDistribution(StableDistribution&&) noexcept = default;
    StableDistribution& operator=(StableDistribution&&) noexcept = default;

    /**
     * Return one random value from distribution.
     *
     * @return Double random value.
     */
    [[nodiscard]] inline double sampleValue() noexcept;

    /**
     * Fill results vector with values sampled from stable distribution.
     *
     * @param results Vector which values (i.e zeros) will be
     *      overwritten by random values from stable distribution.
     */
    void sampleValues(std::vector<double>& results) noexcept;

    /**
     * Return vector of size n filled with values sampled from stable distribution.
     *
     * @param n Resulting vector length.
     * @return Vector of random doubles.
     */
    std::vector<double> sampleValues(std::size_t n) noexcept;

   private:
    double alpha_{.5};
    double gamma_{1};

    std::uniform_real_distribution<> uniform_{-M_PI_2, M_PI_2};
    std::exponential_distribution<> exp_{1};
};

}  // namespace tfbsm

#endif  // TFBSM_STABLEDISTRIBUTION_H_
