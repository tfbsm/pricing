#include <libtfbsm/configuration_repository.hpp>

std::optional<std::shared_ptr<tfbsm::Option>> tfbsm::ConfigurationRepository::get_option_by_spot(std::string const& symbol) const noexcept {
    if (spot_options_.find(symbol) == spot_options_.end()) 
        return std::nullopt;

    return spot_options_.at(symbol);
};

std::optional<std::shared_ptr<tfbsm::Option>> tfbsm::ConfigurationRepository::get_option(std::string const& symbol) const noexcept {
    if (options_.find(symbol) == options_.end()) 
        return std::nullopt;

    return options_.at(symbol);
};

std::optional<std::uint32_t> tfbsm::ConfigurationRepository::get_symbol_code(std::string const& symbol) const noexcept {
    if (symbol_codes_.find(symbol) == symbol_codes_.end())
        return std::nullopt;
    
    return symbol_codes_.at(symbol);
};

void tfbsm::ConfigurationRepository::from_json(nlohmann::json const& j) {
    j["dtau"].get_to(dtau_);
    j["risk_free_rate"].get_to(risk_free_rate_);
    j["buffer_size_for_estimation"].get_to(buffer_size_for_estimation_);

    for (auto const &opt : j["options"]) {
        Option option;
        option.from_json(opt);

        auto ptr = std::make_shared<Option>(option);

        options_.insert({option.symbol, ptr});
        spot_options_.insert({option.spot_symbol, ptr});
    }

    for (auto const &code : j["codes"].items()) {
        symbol_codes_.insert({code.key(), code.value().get<std::uint32_t>()});
    }
}
