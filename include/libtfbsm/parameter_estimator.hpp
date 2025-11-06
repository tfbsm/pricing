#ifndef TFBSM_PARAMETER_ESTIMATOR_H_
#define TFBSM_PARAMETER_ESTIMATOR_H_

namespace tfbsm {

/**
 * Price movement based distribution parameter estimator
 * based on Bayesian problem statement.
 */
class ParameterEstimator {
   public:
    ParameterEstimator();

    ~ParameterEstimator() = default;

    // Parameter estimator stores price data, so it should not be copied.
    ParameterEstimator(ParameterEstimator const&) = delete;
    ParameterEstimator& operator=(ParameterEstimator const&) = delete;
    ParameterEstimator(ParameterEstimator&&) = default;
    ParameterEstimator& operator=(ParameterEstimator&&) = default;

    void onPriceUpdate();
    void estimate();

    [[nodiscard]] double getAlpha();
    [[nodiscard]] double getSigma();

   private:
};

}  // namespace tfbsm

#endif  // TFBSM_PARAMETER_ESTIMATOR_H_
