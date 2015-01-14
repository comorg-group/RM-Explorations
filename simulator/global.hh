#ifndef __GLOBAL_HH__
#define __GLOBAL_HH__

#include <cstdint>

typedef uint64_t Addr;
typedef uint64_t Tick;

struct Request {
    uint64_t id;
    Addr address;
};

#endif
