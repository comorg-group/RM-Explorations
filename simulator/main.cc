#include <iostream>
#include <fstream>
#include <queue>
#include "global.hh"
#include "BaseCache.hh"
#include "BaselineCache.hh"

using namespace std;

int main(int argc, char** argv)
{
    fstream fs;
    fs.open(argv[1]);
    cout << argv[1] << endl;
    if (!fs.is_open())
        return -1;

    uint64_t total_request = 0;
    queue<Request> request_queue;
    while (!fs.eof()) {
        total_request++;
        string op;
        Addr address;
        int useless;
        Tick tick;

        fs >> op >> address >> useless >> useless >> tick;
        Operation operation = (op == "r") ? OpRead : ((op == "w") ? OpWrite : OpUpdate);
        request_queue.push({ operation, total_request, address, tick / 100 });
    }

    printf("aaa\n");

    if (request_queue.empty()) {
        return 0;
    }

    Tick current_tick = request_queue.front().tick;
    Tick total_delay = 0;
    bool cache_available = true;
    auto cache = BaselineCache([&](Request req) {
        printf("popRequest %lld\n", req.id);
        cache_available = true;
        request_queue.pop();
        total_delay += current_tick - req.tick;
    });

    while (!request_queue.empty()) {
        auto current_request = request_queue.front();
        if (cache_available && current_request.tick + 6 <= current_tick) {
            printf("requestCache %lld\n", current_request.id);
            cache_available = false;
            cache.requestCache(current_request);
        }

        current_tick++;
        cache.nextTick(current_tick);
        if (current_tick % 100000 == 0)
            printf("current_tick: %lld\n", current_tick);
    }

    cout << total_delay << endl;

    return 0;
}
