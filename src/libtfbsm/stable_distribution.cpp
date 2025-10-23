#include "libtfbsm/stable_distribution.hpp"

#include <iostream>

inline double tfbsm::StableDistribution::sampleValue() noexcept {
    double U, W, U_0, term1, term2;

    // Will be used to obtain a seed for the random number engine
    thread_local std::random_device rd;

    // Standard mersenne_twister_engine seeded with rd()
    thread_local std::mt19937 gen(rd());

    U = uniform_(gen);
    W = exp_(gen);

    U_0 = -0.5 * M_PI * ((1 - std::abs(1 - alpha_)) / alpha_);

    term1 = std::sin(alpha_ * (U - U_0)) / std::pow(std::cos(U), 1 / alpha_);
    term2 = std::pow(std::cos(U - alpha_ * (U - U_0)) / W, (1 - alpha_) / alpha_);

    return gamma_ * term1 * term2;
}

void tfbsm::StableDistribution::sampleValues(std::vector<double>& results) noexcept {
    for (auto& val : results) {
        val = sampleValue();
    }
}

std::vector<double> tfbsm::StableDistribution::sampleValues(std::size_t n) noexcept {
    std::vector<double> res(n);

    sampleValues(res);

    return res;
}
