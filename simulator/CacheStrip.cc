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

HeadDirection CacheStrip::writeDir(int64_t addr)
{
    Head nearestWriteHead;
    int64_t distance = INT64_MAX;
    for (Head writeHead : writeHeads) {
        if (abs(writeHead + offset - addr) < distance) {
            distance = abs(writeHead + offset - addr);
            nearestWriteHead = writeHead;
        }
    }
    if (distance) {
        return HeadDirection(-(nearestWriteHead + offset - addr) / abs(nearestWriteHead + offset - addr));
    }
    else {
        return STOP;
    }
}

HeadDirection CacheStrip::readDir(int64_t addr)
{
    Head nearestReadHead;
    int64_t distance = INT64_MAX;
    for (Head readHead : readHeads) {
        if (abs(readHead + offset - addr) < distance) {
            distance = abs(readHead + offset - addr);
            nearestReadHead = readHead;
        }
    }
    if (distance) {
        return HeadDirection(-(nearestReadHead + offset - addr) / abs(nearestReadHead + offset - addr));
    }
    else {
        return STOP;
    }
}