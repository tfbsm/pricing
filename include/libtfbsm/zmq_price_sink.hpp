#ifndef TFBSM_ZMQ_PRICE_SINK_H_
#define TFBSM_ZMQ_PRICE_SINK_H_

#include "models.hpp"
#include "price_sink.hpp"

namespace tfbsm
{
    

class ZeroMQPriceSink : PriceSink {
public:
    ZeroMQPriceSink(const std::string& bind_address, 
                    zmq::context_t &ctx,
                    const unsigned int precision) : ctx_(ctx), 
                                           sock_(ctx, zmq::socket_type::pub),
                                           bind_address_(bind_address),
                                           precision_(precision) {};

    ~ZeroMQPriceSink() = default;
    ZeroMQPriceSink(ZeroMQPriceSink const&) = delete;
    ZeroMQPriceSink& operator=(ZeroMQPriceSink const&) = delete;
    ZeroMQPriceSink(ZeroMQPriceSink&&) = default;
    ZeroMQPriceSink& operator=(ZeroMQPriceSink&&) = default;

    void run();
    void onPriceEstimation(const PriceEstimation& estimation) override;
private:
    const unsigned int precision_;
    const std::string& bind_address_;
    zmq::context_t &ctx_;
    zmq::socket_t sock_;
};


} // namespace tfbsm


#endif