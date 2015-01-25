#include "CacheStrip.hh"

bool CacheStrip::writeLine(int64_t addr)
{
    bool ans = false;
    for (Head writeHead : writeHeads) {
        if (writeHead + offset == addr) {
            ans = true;
        }
    }
    if (ans == false) {
        cacheDesignError("strip write error");
    }
    return ans;
}

bool CacheStrip::readLine(int64_t addr)
{
    bool ans = false;
    for (Head readHead : readHeads) {
        if (readHead + offset == addr) {
            ans = true;
        }
    }
    if (ans == false) {
        cacheDesignError("strip read error");
    }
    return ans;
}

bool CacheStrip::shift(HeadDirection dir)
{
    offset += dir;
    return true;
}

HeadDirection CacheStrip::toDir(int64_t addr)
{
    return HeadDirection(sgn(addr - offset));
}

HeadDirection CacheStrip::writeDir(int64_t addr)
{
    Head nearestWriteHead;
    int64_t distance = INT64_MAX;
    for (Head writeHead : writeHeads) {
        if (llabs(writeHead + offset - addr) < llabs(distance)) {
            distance = (writeHead + offset - addr);
            nearestWriteHead = writeHead;
        }
    }
    return HeadDirection(sgn(-distance));
}

HeadDirection CacheStrip::readDir(int64_t addr)
{
    Head nearestReadHead;
    int64_t distance = INT64_MAX;
    for (Head readHead : readHeads) {
        if (llabs(readHead + offset - addr) < llabs(distance)) {
            distance = (readHead + offset - addr);
            nearestReadHead = readHead;
        }
    }
    return HeadDirection(sgn(-distance));
}