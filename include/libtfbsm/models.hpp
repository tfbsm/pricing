#ifndef TFBSM_MODEL_HPP
#define TFBSM_MODEL_HPP

#include <string>
#include <vector>

#include "stdint.h"
#include "time.h"

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

}  // namespace tfbsm

#endif  // TFBSM_MODEL_HPP
