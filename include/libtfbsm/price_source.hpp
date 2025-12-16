#ifndef TFBSM_TICK_COLLECTOR_H_
#define TFBSM_TICK_COLLECTOR_H_


#include "observers.hpp"

namespace tfbsm {

class PriceSource {
public:
    virtual void addTickObserver(TickObserver &) = 0;
    virtual void addKlineObserver(KlineObserver &) = 0;
};

}  // namespace tfbs

#endif