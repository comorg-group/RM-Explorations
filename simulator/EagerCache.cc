#include "EagerCache.hh"
#include "strings.h"

inline uint64_t getTagBits(Addr addr)
{
    return (addr & TAG_MASK) >> __builtin_ctz(TAG_MASK);
}

inline uint64_t getStripBits(Addr addr)
{
    return (addr & STRIP_MASK) >> __builtin_ctz(STRIP_MASK);
}

inline uint64_t getLineBits(Addr addr)
{
    return (addr & LINE_MASK) >> __builtin_ctz(LINE_MASK);
}

inline uint64_t getOffsetBits(Addr addr)
{
    return (addr & OFFSET_MASK) >> __builtin_ctz(OFFSET_MASK);
}

void EagerCache::nextState(Tick tick)
{
    switch (state) {
    case StateIdle:
        break;
    case StateMoving: {
        targetRequest.shiftTime++;
        if (targetOp == OpRead) {
            HeadDirection dir = strip[targetStrip]->readDir(targetAddr);
            strip[targetStrip]->shift(dir);
            if (!strip[targetStrip]->readDir(targetAddr)) {
                changeState(StateReading, tick);
            }
        }
        else {
            HeadDirection dir = strip[targetStrip]->writeDir(targetAddr);
            strip[targetStrip]->shift(dir);
            if (!strip[targetStrip]->writeDir(targetAddr)) {
                changeState(StateWriting, tick);
            }
        }
        break;
    }
    case StateReading: {
        strip[targetStrip]->readDir(targetAddr);
        smu[targetStrip][targetGroup][targetGroupOffset].lastAccessTick = tick;
        callback(targetRequest);
        changeState(StateReturning, tick);
        break;
    }
    case StateWriting: {
        strip[targetStrip]->writeDir(targetAddr);
        smu[targetStrip][targetGroup][targetGroupOffset].lastAccessTick = tick;
        SMUEntry newEntry = { tick, targetTag, true };
        smu[targetStrip][targetGroup][targetGroupOffset] = newEntry;
        changeState(StateReturning, tick);
        break;
    }
    case StateLookup: {
        SMUEntry hitEntry;
        int groupOffset;
        hitEntry.valid = false;
        for (int i = 0; i < GROUPSIZE; i++) {
            SMUEntry entry = smu[targetStrip][targetGroup][i];
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
                    changeState(StateReading, tick);
                }
                else {
                    changeState(StateMoving, tick);
                }
            }
            else {
                callback(targetRequest);
                if (!strip[targetStrip]->writeDir(targetAddr)) {
                    changeState(StateWriting, tick);
                }
                else {
                    changeState(StateMoving, tick);
                }
            }
        }
        else {
            if (targetOp != OpRead) {
                callback(targetRequest);
            }
            int groupOffset;
            Tick oldestTick = 0;
            for (int i = 0; i < GROUPSIZE; i++) {
                SMUEntry entry = smu[targetStrip][targetGroup][i];
                if (!entry.valid || entry.lastAccessTick < oldestTick) {
                    groupOffset = i;
                }
            }
            targetGroupOffset = groupOffset;
            targetAddr = (getLineBits(target) << GROUP_BIT) + targetGroupOffset;
            missCounter = missPenalty;
            changeState(StateMiss, tick);
        }
        break;
    }
    case StateMiss: {
        targetRequest.isMissed = true;
        if (missCounter) {
            missCounter--;
            HeadDirection dir = strip[targetStrip]->writeDir(targetAddr);
            strip[targetStrip]->shift(dir);
            changeState(StateMiss, tick);
        }
        else {
            if (targetOp == OpRead) {
                callback(targetRequest);
            }
            targetOp = OpWrite;
            if (!strip[targetStrip]->writeDir(targetAddr)) {
                changeState(StateWriting, tick);
            }
            else {
                changeState(StateMoving, tick);
            }
        }
        break;
    }
    case StateReturning: {
        int count = 0;
        for (int i = 0; i < (1 << STRIP_BIT); i++) {
            if (strip[i]->offset != 0) {
                strip[i]->shift(strip[i]->toDir(0));
                count++;
                if (count == 2) {
                    break;
                }
            }
        }
        changeState(StateIdle, tick);
        break;
    }
    }
}

int64_t EagerCache::stateLength(State state)
{
    switch (state) {
    case StateIdle:
        return 1;
    case StateMoving:
        return 1;
    case StateReading:
        return 1;
    case StateLookup:
        return 0;
    case StateMiss:
        return 1;
    case StateWriting:
        return 1;
    case StateReturning:
        return 1;
    }
}

void EagerCache::nextTick(Tick tick)
{
    for (int64_t currentMicroTickLeft = microTickPerTick; currentMicroTickLeft >= stateLength(state);) {
        currentMicroTickLeft -= stateLength(state);
        nextState(tick);
    }
}

void EagerCache::requestCache(Request request)
{
    if (state != StateIdle and state != StateReturning) {
        cacheDesignError("new request come while cache is still working!!!");
    }
    else {
        targetOp = request.op;
        targetRequest = request;
        target = request.address;
        targetGroup = getLineBits(target);
        targetStrip = getStripBits(target);
        targetTag = getTagBits(target);
        state = StateLookup;
    }
}

bool EagerCache::isAvailable()
{
    return (state == StateIdle or state == StateReturning);
}

std::string EagerCache::stateToString(State _state)
{
    switch (_state) {
    case StateIdle:
        return "StateIdle";
        break;
    case StateMoving:
        return "StateMoving";
        break;
    case StateReading:
        return "StateReading";
        break;
    case StateWriting:
        return "StateWriting";
        break;
    case StateLookup:
        return "StateLookup";
        break;
    case StateMiss:
        return "StateMiss";
        break;
    case StateReturning:
        return "StateReturning";
        break;
    }
}

void EagerCache::changeState(State _state, Tick tick)
{
    if (state != _state) {
        char text[1000];
        sprintf(text, "@%lld:State change from %s to %s", tick, stateToString(state).c_str(), stateToString(_state).c_str());
        state = _state;
        cacheDesignNotification(text);
    }
}

std::string EagerCache::getStateName()
{
    return stateToString(state);
}