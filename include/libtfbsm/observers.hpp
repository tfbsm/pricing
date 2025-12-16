#ifndef TFBSM_TICK_OBSERVERS_H_
#define TFBSM_TICK_OBSERVERS_H_

#include "models.hpp"

namespace tfbsm
{

class TickObserver {
public:
    virtual void onTick(Tick tick) = 0;
    virtual void onTicks(std::vector<Tick> const& ticks) = 0;
};

class KlineObserver {
public:
    virtual void onKlines(std::vector<OHLC> const& klines) = 0;
};

} // namespace tfbsm


#endif