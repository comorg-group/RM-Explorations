#ifndef __BASE_CACHE_HH__
#define __BASE_CACHE_HH__

#include <functional>
#include "global.hh"

class BaseCache {
    typedef std::function<void(Request)> callback_type;
    callback_type callback;
public:
    BaseCache(callback_type callback) : callback(callback) {}
    virtual void nextTick() = 0;
    virtual void requestCache(Request) = 0;
};

#endif
