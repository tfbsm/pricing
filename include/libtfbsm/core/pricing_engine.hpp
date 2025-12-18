#ifndef TFBSM_PRICING_ENGINE_H_
#define TFBSM_PRICING_ENGINE_H_

#include <libtfbsm/core/models.hpp>
#include <libtfbsm/core/parameter_estimator.hpp>
#include <libtfbsm/core/stable_distribution.hpp>

#include <libtfbsm/interactions/price_sink.hpp>

#include <libtfbsm/configuration_repository.hpp>

namespace tfbsm {

// Time-Fractional Black-Scholes model pricing engine
class PricingEngine : public TickObserver, public KlineObserver {
   public:
    PricingEngine(std::shared_ptr<PriceSink> priceSink,
                  std::unique_ptr<ParameterEstimator> parameterEstimator) noexcept 
                  : parameter_estimator_(std::move(parameterEstimator)), price_sink_(std::move(priceSink)) {
        // distribution = StableDistribution(parameter_estimator_->get_parameters()->alpha,
        //                std::pow(tfbsm::ConfigurationRepository::getInstance().get_dtau(), 
        //                                             1. / parameter_estimator_->get_parameters()->alpha));
    };

    ~PricingEngine() = default;
    PricingEngine(PricingEngine const&) = delete;
    PricingEngine& operator=(PricingEngine const&) = delete;
    PricingEngine(PricingEngine&&) = delete;
    PricingEngine& operator=(PricingEngine&&) = delete;

    void onTick(Tick const& tick) override;
    void onTicks(std::vector<Tick> const& ticks) override;
    void onKlines(std::vector<OHLC> const& klines) override;

   private:
    StableDistribution distribution;
    std::shared_ptr<PriceSink> price_sink_;
    std::unique_ptr<ParameterEstimator> parameter_estimator_;

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
