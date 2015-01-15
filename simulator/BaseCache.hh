#ifndef __BASE_CACHE_HH__
#define __BASE_CACHE_HH__

#include <functional>
#include "global.hh"

typedef std::function<void(Request)> callback_type;

class BaseCache {
protected:
    callback_type callback;
    static const Tick missPenalty = 100;
    static const Tick accessLatency = 6;
public:
    BaseCache(callback_type callback) : callback(callback) {}
    virtual void nextTick(Tick) = 0;
    virtual void requestCache(Request) = 0;
};

#endif
