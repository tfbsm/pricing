#ifndef TFBSM_OPTION_DTO_HPP
#define TFBSM_OPTION_DTO_HPP
#include "stdint.h"
#include "string"

#include "nlohmann/json.hpp"

namespace tfbsm {

class Option {
public:
    enum class OptionType : std::uint8_t {kCall, kPut};
    OptionType type; 
    std::string symbol;

    double strike_price;

    time_t expiration_time_stamp;

    Option() = default;

    Option(OptionType type, double strike_price, time_t expiration_time_stamp)
            : type(type), strike_price(strike_price), expiration_time_stamp(expiration_time_stamp){};

    
    void from_json(const nlohmann::json& j) {
        j.at("symbol").get_to(symbol);
        j.at("strike_price").get_to(strike_price);
        j.at("expiration").get_to(expiration_time_stamp);

        auto _type = j.at("type").get<std::string>();
        if (_type == "call") {
            type = OptionType::kCall;
        } else if (_type == "put") {
            type = OptionType::kPut;
        }
    }

    void to_json(nlohmann::json& j) const noexcept {
        auto type_str = type == OptionType::kCall ? "call" : "put";

        j = nlohmann::json{
            {"symbol", symbol},
            {"type", type_str},
            {"strike_price", strike_price},
            {"expiration", expiration_time_stamp}
        };
    }
};

}  // namespace tfbsm

#endif  // TFBSM_OPTION_DTO_HPP