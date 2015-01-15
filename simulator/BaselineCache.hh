#ifndef __BASELINE_CACHE_HH__
#define __BASELINE_CACHE_HH__

#include "BaseCache.hh"
#include "CacheStrip.hh"

#define OFFSET_BIT 9
#define OFFSET_MASK 0x000001FF
#define LINE_BIT 3
#define LINE_MASK 0x00000E00
#define STRIP_BIT 10
#define STRIP_MASK 0x3FFF000
#define TAG_BIT 7
#define TAG_MASK 0xFC000000
#define GROUP_BIT 3
#define GROUPSIZE 8

struct SMUEntry {
    Tick lastAccessTick;
    uint64_t tag;
    bool valid;
};

class BaselineCache : public BaseCache{
private:
    typedef enum : uint8_t {
        StateIdle,
        StateMoving,
        StateReading,
        StateLookup,
        StateMiss,
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
    bool targetHot;
    Operation targetOp;
    CacheStrip * strip[1<<STRIP_BIT];
    SMUEntry smu[1<<STRIP_BIT][1<<LINE_BIT][GROUPSIZE];
public:
    BaselineCache(callback_type callback):BaseCache(callback){
        for (int i=0 ; i<(1<<STRIP_BIT); i++) {
            strip[i]= new CacheStrip({0,16,32,48}, {0,16,32,48});
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
};

#endif