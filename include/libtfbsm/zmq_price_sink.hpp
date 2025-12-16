#ifndef TFBSM_ZMQ_PRICE_SINK_H_
#define TFBSM_ZMQ_PRICE_SINK_H_

#include "models.hpp"
#include "price_sink.hpp"

namespace tfbsm {

class ZeroMQPriceSink : public PriceSink {
   public:
    ZeroMQPriceSink(std::string const& bind_address, zmq::context_t& ctx,
                    unsigned int const precision)
        : 
        precision_(precision),
        bind_address_(bind_address),
        sock_(ctx, zmq::socket_type::pub) {};

    ~ZeroMQPriceSink() = default;
    ZeroMQPriceSink(ZeroMQPriceSink const&) = delete;
    ZeroMQPriceSink& operator=(ZeroMQPriceSink const&) = delete;
    ZeroMQPriceSink(ZeroMQPriceSink&&) = delete;
    ZeroMQPriceSink& operator=(ZeroMQPriceSink&&) = delete;

    void run();
    void onPriceEstimation(PriceEstimation const& estimation) override;

   private:
    unsigned int const precision_;
    std::string const bind_address_;
    zmq::socket_t sock_;
};

}  // namespace tfbsm

#endif