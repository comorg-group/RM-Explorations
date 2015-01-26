# Explorations on building a cache system based on Racetrack Memory
## RaceTrack Cache simulator design and system optimization

杨俊睿 1200012860
刘当一 1100011625

//TODO: added personal picture

### Introduction

The wide adoption of chip multiprocessors (CMPs) has generated an explosive demand for on-chip memory resources. Unfortunately, as technology scaling continues, the reliance on conventional SRAM technology for on-chip caches raises
several concerns including the large memory cell size, the high leakage power consumption, and the low resilience to soft errors. A very recent emerging nonvolatile memory technology, the racetrack memory [x], has been projected to over come the density barrier and high leakage power of existing memory solutions.[x]

//TODO: rephrase the paragraph above
//TODO: about other studies in racetrack memory

### Basics of Racetrack Memory

Figure 1 showed a basic structure of a racetrack strip, which consist of a nano wire and several reading and writing port. The nano wire has many magnetic domains, where the information stores at, and they are separated by narrow domain walls for prevention of wrong access. The nano wire is driven by shift ctrl, allowing the information stored inside those domains to shift across the nano wire. Some access port is placed across the nano wire in order to read and write data on the wire. Typically, because of the large size of access ports comparing to the domains on the nano wire, there are much more domains than access ports. Traditionally, only RW-port is build enabling both write and read access to the domains. Recent development propose R port and W port in a smaller size for more flexibility in design.

//TODO: Add figure 1

### Design
#### Design of simulator

//TODO: Add figure 2
Our simulator exactly mimic the CPU cache interaction. Our simulator is composed of two parts as depicted in Figure 2. The part on the left is the main simulation sequence control logic in `main` function, which send the request recorded in trace file in the exact same order and time. The part on the right is the cache module, which have different implementation for different cache. All cache module inherit a base class called as `BaseCache`.

#### Design of cache

//TODO: liudangyi

### Implementation

#### `main` function

//TODO: liudangyi


#### Interface defined in `BaseCache`

Below is code adopted from `BaseCache.hh`

```c++
class BaseCache {
public:
    typedef std::function<void(Request)> callback_type;
    BaseCache(callback_type callback) : callback(callback) {}
    virtual void nextTick(Tick) = 0;
    virtual void requestCache(Request) = 0;
    virtual bool isAvailable() = 0;
#ifdef STAT
    virtual std::string getStateName() = 0;
#endif
protected:
    callback_type callback;
    static const Tick missPenalty = 100;
    static const Tick accessLatency = 6;
};
```

The usage of interface is shown in figure 3.

//TODO: add figure 3

#### Cache implementation

//TODO: yangjunrui

### Experiment

//TODO:

### Conclusion

### Future work

