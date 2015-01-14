#include <iostream>
#include <fstream>
#include "global.hh"
#include "BaseCache.hh"

using namespace std;

int main(int argc, char** argv) {
    fstream fs;
    fs.open(argv[1]);
    if(!fs.is_open())
        return -1;

    BaseCache &cache;
    Tick last_tick = 0;
    uint64_t request_id = 0;

    while (!fs.eof()) {
        request_id++;
        string op;
        Addr address;
        int useless;
        Tick tick;

        fs >> op >> address >> useless >> useless >> tick;

        if (last_tick == 0)
            last_tick = tick;
        for (int i = 0; i < tick - last_tick; i++)
            cache.nextTick();

        cache.requestCache(Request({request_id, address}));
    }

    return 0;
}
