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

    std::optional<std::shared_ptr<Option>> get_option_by_spot(std::string const& symbol) const noexcept;
    std::optional<std::shared_ptr<Option>> get_option(std::string const& symbol) const noexcept;
    std::optional<std::uint32_t> get_symbol_code(std::string const& symbol) const noexcept;

    void from_json(nlohmann::json const& j);

private:
    ConfigurationRepository() : risk_free_rate_(0.0), dtau_(1e-5), buffer_size_for_estimation_(10) {
    }
    double risk_free_rate_;
    double dtau_;
    uint16_t buffer_size_for_estimation_;
    std::unordered_map<std::string, std::shared_ptr<Option>> options_;
    std::unordered_map<std::string, std::shared_ptr<Option>> spot_options_;
    std::unordered_map<std::string, std::uint32_t> symbol_codes_;
};

}  // namespace tfbsm

#endif  // TFBSM_CONFIGURATION_REPOSITORY_H_