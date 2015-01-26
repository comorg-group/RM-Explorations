#ifndef __MASSIVE_CACHE_HH__
#define __MASSIVE_CACHE_HH__

#include "BaseCache.hh"
#include "CacheStrip.hh"

#define OFFSET_BIT 6
#define OFFSET_MASK 0x3F
#define LINE_BIT 3
#define LINE_MASK 0x1C0
#define STRIP_BIT 10
#define STRIP_MASK 0x7FE00
#define TAG_BIT 13
#define TAG_MASK 0xFFF80000
#define GROUP_BIT 3
#define GROUPSIZE 8

class MassiveCache : public BaseCache {
    struct SMUEntry {
        Tick lastAccessTick;
        uint64_t tag;
        bool valid;
    };
private:
    typedef enum : uint8_t {
        StateIdle,
        StateMoving,
        StateReading,
        StateLookup,
        StateMiss,
        StateWriting,
        StateReturning,
    } State;
    State state;
    Request targetRequest;
    uint64_t target; // Target address passed by request
    
    //Target = targetTag:TargetStrip:TargetGroup:offset
    uint64_t targetGroup;
    uint64_t targetStrip;
    uint64_t targetTag;
    
    //TargetAddr = targetGroup:targetGroupOffset
    uint64_t targetAddr;
    uint64_t targetGroupOffset;
    
    uint32_t missCounter;
    Operation targetOp;
    CacheStrip * strip[1<<STRIP_BIT];
    SMUEntry smu[1<<STRIP_BIT][1<<LINE_BIT][GROUPSIZE];
    void changeState(State, Tick);
    std::string stateToString(State);
    
    static const int64_t microTickPerTick = 1;
    void nextState(Tick);
    int64_t stateLength(State);
public:
    MassiveCache(callback_type callback):BaseCache(callback){
        for (int i=0 ; i<(1<<STRIP_BIT); i++) {
            strip[i]= new CacheStrip({2,6,10,14,18,22,26,30,34,38,42,46,50,54,58,62}, {2,62});
        }
        for (int i=0; i<(1<<STRIP_BIT); i++) {
            for (int j=0; j<(1<<LINE_BIT); j++) {
                for (int k=0; k<GROUPSIZE; k++) {
                    smu[i][j][k].valid = false;
                }
            }
        }
    }
    virtual void nextTick(Tick tick);
    virtual void requestCache(Request);
    virtual bool isAvailable();
    virtual std::string getStateName();
};

#endif