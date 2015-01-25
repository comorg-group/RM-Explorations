#include "BaselineCache.hh"
#include "strings.h"

uint64_t getTagBits(Addr addr)
{
    return (addr & TAG_MASK) >> __builtin_ctz(TAG_MASK);
}

uint64_t getStripBits(Addr addr)
{
    return (addr & STRIP_MASK) >> __builtin_ctz(STRIP_MASK);
}

uint64_t getLineBits(Addr addr)
{
    return (addr & LINE_MASK) >> __builtin_ctz(LINE_MASK);
}

uint64_t getOffsetBits(Addr addr)
{
    return (addr & OFFSET_MASK) >> __builtin_ctz(OFFSET_MASK);
}

void BaselineCache::nextTick(Tick tick)
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
        changeState(StateIdle, tick);
        break;
    }
    case StateWriting: {
        strip[targetStrip]->writeDir(targetAddr);
        smu[targetStrip][targetGroup][targetGroupOffset].lastAccessTick = tick;
        SMUEntry newEntry = { tick, targetTag, true };
        smu[targetStrip][targetGroup][targetGroupOffset] = newEntry;
        changeState(StateIdle, tick);
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
        targetOp = request.op;
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

std::string BaselineCache::stateToString(State _state)
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
    }
}

void BaselineCache::changeState(State _state, Tick tick)
{
    if (state != _state) {
        char text[1000];
        sprintf(text, "@%lld:State change from %s to %s", tick, stateToString(state).c_str(), stateToString(_state).c_str());
        state = _state;
        cacheDesignNotification(text);
    }
}