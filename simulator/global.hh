#ifndef __GLOBAL_HH__
#define __GLOBAL_HH__

#include <cstdint>
#include <iostream>

typedef uint64_t Addr;
typedef uint64_t Tick;

#if 1
#define debug(...)
#else
#define debug(...) do { \
        printf("\033[36m[DEBUG] %s", __FUNCTION__); \
        printf(__VA_ARGS__); \
        printf("\033[0m\n"); \
    } while (0)
#endif

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
};

inline void cacheDesignError(const std::string& string){
    debug("%s", string.c_str());
}

inline void cacheDesignNotification(const std::string& string){
    debug("%s", string.c_str());
}

#endif
