#include <chrono>
#include <iostream>

#include "libtfbsm/model.hpp"
#include "libtfbsm/pricing_engine.hpp"
#include "libtfbsm/stable_distribution.hpp"

typedef std::chrono::steady_clock::time_point tp;
typedef std::chrono::steady_clock clock_;

int main(int argc, char const* argv[]) {
    // Create ZeroMQ context
    zmq::context_t context(1);
    // tfbsm::StableDistribution sd{0.5, 1.0};

    // tfbsm::PricingEngine pe{.5, 1e-2};

    // std::vector<double> values(1e5);

    // clock_::time_point begin = clock_::now();

    // for (int i = 0; i < 1e3; ++i) {
    //     sd.sampleValues(values);
    // }

    // clock_::time_point end = clock_::now();

    // std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() / 1e3
    //           << std::endl;

    return 0;
}
