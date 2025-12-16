#ifndef TFBSM_ZMQ_PRICE_SOURCE_H_
#define TFBSM_ZMQ_PRICE_SOURCE_H_

#include "zmq.hpp"
#include "spdlog/spdlog.h"

#include "models.hpp"
#include "price_source.hpp"

#include <shared_mutex>
#include <unordered_set>
#include <thread>
#include <iostream>

namespace tfbsm
{
    
class ZeroMQPriceSource : public PriceSource {
public:
    ZeroMQPriceSource(std::string const& address,
                      zmq::context_t& ctx,
                      zmq::socket_type socket_type = zmq::socket_type::sub) 
                      : ctx_(ctx),
                        sock_(ctx, socket_type) {
        spdlog::debug("ZeroMQ price source connecting to {}", address);

        sock_.connect(address);
        sock_.setsockopt(ZMQ_SUBSCRIBE, "", 0); // subscribe to everything
        
        receiver_ = std::thread(&ZeroMQPriceSource::run, this);
        receiver_.detach();
    };
    
    ~ZeroMQPriceSource() {
        if (receiver_.joinable())
            receiver_.join();
    };

    ZeroMQPriceSource(ZeroMQPriceSource const&) = delete;
    ZeroMQPriceSource& operator=(ZeroMQPriceSource const&) = delete;
    ZeroMQPriceSource(ZeroMQPriceSource&&) = delete;
    ZeroMQPriceSource& operator=(ZeroMQPriceSource &&) = delete;

    void addTickObserver(TickObserver &obs) override {
        std::unique_lock<std::shared_mutex> guard(mtx_);

        tick_oververs_.insert(&obs);
    };

    void addKlineObserver(KlineObserver &obs) override {
        std::unique_lock<std::shared_mutex> guard(mtx_);

        kline_observers_.insert(&obs);
    };

private:
    void run();

    std::thread receiver_;
    zmq::context_t& ctx_;
    zmq::socket_t sock_;

    std::shared_mutex mtx_;
    std::unordered_set<TickObserver*> tick_oververs_;
    std::unordered_set<KlineObserver*> kline_observers_;
};

} // namespace tfbsm


#endif