#ifndef TFBSM_CONFIGURATION_REPOSITORY_H_
#define TFBSM_CONFIGURATION_REPOSITORY_H_

#include "nlohmann/json.hpp"

#include <libtfbsm/core/models.hpp>
#include <memory>
#include <mutex>

namespace tfbsm {
    
class ConfigurationRepository {
public: 
    static ConfigurationRepository& getInstance() {
        static ConfigurationRepository instance;
        return instance;
    }
    
    double get_risk_free_rate() const noexcept { return risk_free_rate_; }
    double get_dtau() const noexcept { return dtau_; }
    uint16_t get_buffer_size_for_estimation() const noexcept { return buffer_size_for_estimation_; }
    
    std::optional<Option> get_option(std::string const& symbol) const noexcept {
        if (options_.find(symbol) == options_.end()) 
            return std::nullopt;

        return options_.at(symbol);
    };

    void from_json(nlohmann::json const& j) {
        j["dtau"].get_to(dtau_);
        j["risk_free_rate"].get_to(risk_free_rate_);
        j["buffer_size_for_estimation"].get_to(buffer_size_for_estimation_);

        for (auto const &opt : j["options"]) {
            Option option;
            option.from_json(opt);

            options_.insert({option.symbol, option});
        }
    }

private:
    ConfigurationRepository() : risk_free_rate_(0.0), dtau_(1e-5), buffer_size_for_estimation_(10) {
    }
    double risk_free_rate_;
    double dtau_;
    uint16_t buffer_size_for_estimation_;
    std::unordered_map<std::string, Option> options_;
};

}  // namespace tfbsm

#endif  // TFBSM_CONFIGURATION_REPOSITORY_H_