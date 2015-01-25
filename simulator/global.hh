#ifndef __GLOBAL_HH__
#define __GLOBAL_HH__

#include <cstdint>
#include <iostream>

typedef uint64_t Addr;
typedef uint64_t Tick;

#define STAT
// #define DEBUG
#ifndef DEBUG
#define debug(...)
#else
#define debug(...) do { \
        printf("\033[36m[DEBUG] %s", __FUNCTION__); \
        printf(__VA_ARGS__); \
        printf("\033[0m\n"); \
    } while (0)
#endif

#define error(...) do { \
        printf("\033[31m[ERROR] %s", __FUNCTION__); \
        printf(__VA_ARGS__); \
        printf("\033[0m\n"); \
        exit(-1); \
    } while (0)

typedef enum : uint8_t {
    OpRead,
    OpWrite,
    OpUpdate,
} Operation;

struct Request {
    Operation op;
    uint64_t id;
    Addr address;
    Tick tick;
    Tick shiftTime;
    bool isMissed;
};

inline void cacheDesignError(const std::string& string){
    error("%s", string.c_str());
}

inline void cacheDesignNotification(const std::string& string){
    debug("%s", string.c_str());
}

template <typename T> int64_t sgn(T val) {
    return (T(0) < val) - (val < T(0));
}

#endif
