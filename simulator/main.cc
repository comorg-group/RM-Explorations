#include <iostream>
#include <fstream>
#include "global.hh"
#include "BaseCache.hh"
#include "BaselineCache.hh"

using namespace std;

int main(int argc, char** argv)
{
    fstream fs;
    fs.open(argv[1]);
    if (!fs.is_open())
        return -1;
    callback_type callback = [](Request req) {};
    BaselineCache blCache = BaselineCache(callback);
    BaseCache& cache = blCache;
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
            cache.nextTick(tick);
        Operation operation = (op == "r") ? OpRead : ((op == "w") ? OpWrite : OpUpdate);
        Request request = { operation, request_id, address };
        cache.requestCache(request);
    }

    return 0;
}
