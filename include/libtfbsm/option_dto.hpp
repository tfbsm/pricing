#ifndef TFBSM_OPTION_DTO_HPP
#define TFBSM_OPTION_DTO_HPP
#include "stdint.h"
#include "string"

namespace tfbsm {

class OptionDto {
public:
    enum class OptionType {
        Call,
        Put
    };
    OptionType type; 
    std::string symbol;

    double strike_price;
    double market_price;

    time_t expiration_time_stamp;

    OptionDto(OptionType type, double strike_price, double market_price, time_t expiration_time_stamp)
            : type(type), strike_price(strike_price), market_price(market_price), expiration_time_stamp(expiration_time_stamp){};
};

}  // namespace tfbsm

#endif  // TFBSM_OPTION_DTO_HPP