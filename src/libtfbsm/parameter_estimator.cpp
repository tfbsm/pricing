#include "libtfbsm/parameter_estimator.hpp"
#include "libtfbsm/models.hpp"
#include "libtfbsm/configuration_repository.hpp"

void tfbsm::ParameterEstimator::OptionsBufferIsFull() {
    if(options_buffer.size() >=  tfbsm::ConfigurationRepository::getInstance().get_buffer_size_for_estimation()){
        estimate();
        options_buffer.clear();
    }
}

void tfbsm::ParameterEstimator::onTick(tfbsm::Tick tick){
    tfbsm::ParameterEstimator::options_buffer.push_back(
            std::pair<time_t, double>(tick.ts, (tick.bid + tick.ask)/2));
    OptionsBufferIsFull();
};

void tfbsm::ParameterEstimator::onTicks(std::vector<Tick> const& ticks){
    options_buffer.reserve(options_buffer.size() + ticks.size());
    
    std::transform(
        ticks.begin(), 
        ticks.end(), 
        std::back_inserter(options_buffer),
        [](const Tick& tick) {
            return std::make_pair(tick.ts, (tick.bid + tick.ask) / 2.0);
        }
    );
    OptionsBufferIsFull();
};

void tfbsm::ParameterEstimator::onKlines(std::vector<OHLC> const& klines){
    options_buffer.reserve(options_buffer.size() + klines.size());
    
    std::transform(
        klines.begin(), 
        klines.end(), 
        std::back_inserter(options_buffer),
        [](const OHLC& klines) {
            return std::make_pair(klines.ts, klines.close);
        }
    );
    OptionsBufferIsFull();
};

void onPriceUpdate();

double CaputoL1Approximation(
    const std::vector<double>& v,
    const std::vector<double>& t,
    std::size_t n,
    double alpha)
{
    double sum = 0.0;
    for (std::size_t k = 0; k < n; ++k)
    {
        double dt = t[k+1] - t[k];
        double w =
            std::pow(t[n] - t[k], 1.0 - alpha)
          - std::pow(t[n] - t[k+1], 1.0 - alpha);

        sum += (v[k+1] - v[k]) / dt * w;
    }
    return sum / std::tgamma(2.0 - alpha);
}

double LossFunction(
    const std::vector<double>& v,
    const std::vector<double>& t,
    double alpha,
    double sigma)
{
    double err = 0.0;
    for (std::size_t n = 1; n < v.size(); ++n)
    {
        double dcap = CaputoL1Approximation(v, t, n, alpha);
        double r = dcap + sigma * sigma * v[n];
        err += r * r;
    }
    return err / v.size();
}

void tfbsm::ParameterEstimator::estimate(){
    if (options_buffer.size() < 10)
        return;

    std::sort(options_buffer.begin(), options_buffer.end(),
              [](auto& a, auto& b){ return a.first < b.first; });

    const std::size_t N = options_buffer.size();

    std::vector<double> t(N), v(N);
    for (std::size_t i = 0; i < N; ++i)
    {
        t[i] = double(options_buffer[i].first);
        v[i] = options_buffer[i].second;
    }

    double best_alpha = 1.0;
    double best_sigma = 0.2;
    double best_loss = std::numeric_limits<double>::max();

    // grid search instead of gradient
    for (double alpha = 0.1; alpha <= 1.0; alpha += 0.01)
    {
        for (double sigma = 0.05; sigma <= 1.0; sigma += 0.01)
        {
            double J = LossFunction(v, t, alpha, sigma);
            if (J < best_loss)
            {
                best_loss = J;
                best_alpha = alpha;
                best_sigma = sigma;
            }
        }
    }

    parameters_.alpha = best_alpha;
    parameters_.sigma = best_sigma;
}


[[nodiscard]] tfbsm::ParameterEstimator::Parameters getParameters();

