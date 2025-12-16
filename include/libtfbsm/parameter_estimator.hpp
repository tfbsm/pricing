#ifndef TFBSM_PARAMETER_ESTIMATOR_H_
#define TFBSM_PARAMETER_ESTIMATOR_H_

#include "models.hpp"
#include "observers.hpp"

#include <optional>

namespace tfbsm {

/**
 * Price movement based distribution parameter estimator
 * based on Bayesian problem statement.
 */
class ParameterEstimator : public TickObserver, KlineObserver {
   public:
    struct Parameters
    {
      double alpha;
      double sigma; 
    };

    ParameterEstimator() {};

    ~ParameterEstimator() = default;

    // Parameter estimator stores price data, so it should not be copied.
    ParameterEstimator(ParameterEstimator const&) = default;
    ParameterEstimator& operator=(ParameterEstimator const&) = default;
    ParameterEstimator(ParameterEstimator&&) = default;
    ParameterEstimator& operator=(ParameterEstimator&&) = default;

    void onTick(Tick const& tick) override;
    void onTicks(std::vector<Tick> const& ticks) override;
    void onKlines(std::vector<OHLC> const& klines) override;


    void estimate();

    [[nodiscard]] std::optional<Parameters> get_parameters() const noexcept {return parameters_;}

   private:
      Parameters parameters_;
      std::vector<std::pair<time_t, double>> options_buffer;
      void OptionsBufferIsFull();
};

}  // namespace tfbsm

#endif  // TFBSM_PARAMETER_ESTIMATOR_H_
