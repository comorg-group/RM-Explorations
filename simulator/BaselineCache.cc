#include "BaselineCache.hh"
#include "strings.h"

uint64_t getTagBits(Addr addr)
{
    return (addr & TAG_MASK) >> __builtin_ffs(TAG_MASK);
}

uint64_t getStripBits(Addr addr)
{
    return (addr & STRIP_MASK) >> __builtin_ffs(STRIP_MASK);
}

uint64_t getLineBits(Addr addr)
{
    return (addr & LINE_MASK) >> __builtin_ffs(LINE_MASK);
}

uint64_t getOffsetBits(Addr addr)
{
    return (addr & OFFSET_MASK) >> __builtin_ffs(OFFSET_MASK);
}

void BaselineCache::nextTick(Tick tick)
{
    switch (state) {
    case StateIdle:
        break;
    case StateMoving: {
        if (targetOp == OpRead) {
            HeadDirection dir = strip[targetStrip]->readDir(targetAddr);
            strip[targetStrip]->shift(dir);
            if (!strip[targetStrip]->readDir(targetAddr)) {
                state = StateReading;
            }
        }
        else {
            HeadDirection dir = strip[targetStrip]->writeDir(targetAddr);
            strip[targetStrip]->shift(dir);
            if (!strip[targetStrip]->writeDir(targetAddr)) {
                state = StateWriting;
            }
        }
        break;
    }
    case StateReading: {
        strip[targetStrip]->readDir(targetAddr);
        smu[targetStrip][targetGroup][targetGroupOffset].lastAccessTick = tick;
        callback(targetRequest);
        state = StateIdle;
        break;
    }
    case StateWriting: {
        strip[targetStrip]->writeDir(targetAddr);
        smu[targetStrip][targetGroup][targetGroupOffset].lastAccessTick = tick;
    }
    case StateLookup: {
        SMUEntry hitEntry;
        int groupOffset;
        hitEntry.valid = false;
        for (int i = 0; i < GROUPSIZE; i++) {
            SMUEntry entry = smu[targetStrip][targetAddr][i];
            if (entry.valid && targetTag == entry.tag) {
                hitEntry = entry;
                groupOffset = i;
                break;
            }
        }
        if (hitEntry.valid) {
            targetGroupOffset = groupOffset;
            targetAddr = (getLineBits(target) << GROUP_BIT) + groupOffset;
            if (targetOp == OpRead) {
                if (!strip[targetStrip]->readDir(targetAddr)) {
                    state = StateReading;
                }
                else {
                    state = StateMoving;
                }
            }
            else {
                callback(targetRequest);
                if (!strip[targetStrip]->writeDir(targetAddr)) {
                    state = StateWriting;
                }
                else {
                    state = StateMoving;
                }
            }
        }
        else {
            missCounter = missPenalty;
            state = StateMiss;
        }
        break;
    }
    case StateMiss: {
        if (!missCounter) {
            missCounter--;
            state = StateMiss;
        }
        else {
            SMUEntry oldEntry;
            int groupOffset;
            Tick oldestTick = 0;
            for (int i = 0; i < GROUPSIZE; i++) {
                SMUEntry entry = smu[targetStrip][targetAddr][i];
                if (!entry.valid || entry.lastAccessTick < oldestTick) {
                    oldEntry = entry;
                    groupOffset = i;
                }
            }
            SMUEntry newEntry = { tick, targetTag, true };
            smu[targetStrip][targetAddr][groupOffset] = newEntry;
            targetOp = OpWrite;
            callback(targetRequest);
            if (!strip[targetStrip]->writeDir(targetAddr)) {
                state = StateWriting;
            }
            else {
                state = StateMoving;
            }
        }
        break;
    }
    default:
        cacheDesignError("State not implemented!");
        break;
    }
}

void BaselineCache::requestCache(Request request)
{
    if (state != StateIdle) {
        cacheDesignError("new request come while cache is still working!!!");
    }
    else {
        targetRequest = request;
        target = request.address;
        targetGroup = getLineBits(target);
        targetStrip = getStripBits(target);
        targetTag = getTagBits(target);
        state = StateLookup;
    }
}

bool BaselineCache::isAvailable()
{
    return (state == StateIdle);
}