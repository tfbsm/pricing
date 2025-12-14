#ifndef TFBSM_PARAMETER_ESTIMATOR_H_
#define TFBSM_PARAMETER_ESTIMATOR_H_

namespace tfbsm {

/**
 * Price movement based distribution parameter estimator
 * based on Bayesian problem statement.
 */
class ParameterEstimator {
   public:
    struct Parameters
    {
      double alpha;
      double sigma; 
    };
    

    ParameterEstimator();

    ~ParameterEstimator() = default;

    // Parameter estimator stores price data, so it should not be copied.
    ParameterEstimator(ParameterEstimator const&) = delete;
    ParameterEstimator& operator=(ParameterEstimator const&) = delete;
    ParameterEstimator(ParameterEstimator&&) = default;
    ParameterEstimator& operator=(ParameterEstimator&&) = default;

    virtual void onPriceUpdate();
    virtual void estimate();

    [[nodiscard]] virtual Parameters getParameters();

   private:
};

}  // namespace tfbsm

#endif  // TFBSM_PARAMETER_ESTIMATOR_H_
