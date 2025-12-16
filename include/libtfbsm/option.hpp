#ifndef TFBSM_OPTION_DTO_HPP
#define TFBSM_OPTION_DTO_HPP
#include "stdint.h"
#include "string"

namespace tfbsm {

class Option {
public:
    enum class OptionType : std::uint8_t {kCall, kPut};
    OptionType type; 
    std::string symbol;

    double strike_price;

    time_t expiration_time_stamp;

    Option(OptionType type, double strike_price, time_t expiration_time_stamp)
            : type(type), strike_price(strike_price), expiration_time_stamp(expiration_time_stamp){};
};

}  // namespace tfbsm

#endif  // TFBSM_OPTION_DTO_HPP