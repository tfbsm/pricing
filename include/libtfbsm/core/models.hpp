#ifndef TFBSM_MODEL_HPP
#define TFBSM_MODEL_HPP

#include <string>
#include <vector>

#include "stdint.h"
#include "time.h"

#include "nlohmann/json.hpp"

namespace tfbsm {

struct OHLC {
    time_t ts;
    uint32_t timescale_s;
    std::string symbol;
    double open, high, low, close, volume;
};

struct Tick {
    enum class Market : std::uint8_t {
        kOption,
        kSpot,
    };

    time_t ts;
    std::string symbol;
    Market market;
    double bid, ask;
};


// std::ostream& operator<<(std::ostream& os, Tick const& t) {
//     os << "Tick(market=" << (t.market == Tick::Market::kOption ? "option" : "spot")
//        << ", ts=" << t.ts << ", symbol=" << t.symbol << ", bid=" << t.bid << ", ask=" << t.ask
//        << ")";

//     return os;
// }

struct PriceEstimation {
    time_t ts;
    std::uint32_t spot_symbol_code;
    std::uint32_t option_symbol_code;
    double market_price;
    double estimated_price;
};

class Model {
   private:
    /* data */
    int test_field;

   public:
    Model(int test_arg = 123) : test_field(test_arg) {}

    void do_something();
    int getTestField() const noexcept { return test_field; }
};

class Option {
public:
    enum class OptionType : std::uint8_t {kCall, kPut};
    OptionType type; 
    std::string symbol;
    std::string spot_symbol;

    double strike_price;

    time_t expiration_time_stamp;

    Option() = default;

    // Option(OptionType type, double strike_price, time_t expiration_time_stamp)
    //         : type(type), strike_price(strike_price), expiration_time_stamp(expiration_time_stamp){};

    
    void from_json(const nlohmann::json& j) {
        j.at("symbol").get_to(symbol);
        j.at("spot_symbol").get_to(spot_symbol);
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
            {"spot_symbol", spot_symbol},
            {"type", type_str},
            {"strike_price", strike_price},
            {"expiration", expiration_time_stamp}
        };
    }
};

}  // namespace tfbsm

#endif  // TFBSM_MODEL_HPP
