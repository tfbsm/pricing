#ifndef TFBSM_PRICE_SINK_H_
#define TFBSM_PRICE_SINK_H_

#include "zmq.hpp"

#include <libtfbsm/core/models.hpp>

namespace tfbsm {

class PriceSink {
   public:
    virtual void onPriceEstimation(PriceEstimation const& estimation) = 0;
};

}  // namespace tfbsm

#endif