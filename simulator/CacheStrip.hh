#ifndef __CACHE_STRIP_HH__
#define __CACHE_STRIP_HH__

#include "BaseCache.hh"
#include <vector>
#include <set>

typedef int64_t Head;

typedef enum : int64_t {
    UP = -1,
    STOP = 0,
    DOWN = 1,
} HeadDirection;

class CacheStrip{
public:
    CacheStrip(std::set<Head> _readHeads, std::set<Head> _writeHeads, int64_t _offset = 0): readHeads(_readHeads), writeHeads(_writeHeads), offset(_offset) {};
    std::set<Head> readHeads;
    std::set<Head> writeHeads;
    int64_t offset;
    bool writeLine(int64_t);
    bool readLine(int64_t);
    bool shift(HeadDirection);
    virtual HeadDirection writeDir(int64_t);
    virtual HeadDirection readDir(int64_t);
};

#endif