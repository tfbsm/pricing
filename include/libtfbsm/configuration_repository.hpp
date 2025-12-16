#ifndef TFBSM_CONFIGURATION_REPOSITORY_H_
#define TFBSM_CONFIGURATION_REPOSITORY_H_

#include "libtfbsm/option.hpp"
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
    Option get_option(std::string symbol) const noexcept;
    
private:
    ConfigurationRepository() : risk_free_rate_(0.0), dtau_(1e-5), buffer_size_for_estimation_(10) {
    }
    double risk_free_rate_;
    double dtau_;
    uint16_t buffer_size_for_estimation_;
};

}  // namespace tfbsm

#endif  // TFBSM_CONFIGURATION_REPOSITORY_H_