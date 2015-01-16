#ifndef __BASE_CACHE_HH__
#define __BASE_CACHE_HH__

#include <functional>
#include "global.hh"

class BaseCache {
public:
    typedef std::function<void(Request)> callback_type;
    BaseCache(callback_type callback) : callback(callback) {}
    virtual void nextTick(Tick) = 0;
    virtual void requestCache(Request) = 0;
    virtual bool isAvailable() = 0;
protected:
    callback_type callback;
    static const Tick missPenalty = 100;
    static const Tick accessLatency = 6;
};

#endif
