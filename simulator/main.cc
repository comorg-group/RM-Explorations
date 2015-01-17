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
        request_queue.push({ operation, total_request, address, tick / 1000 });
    }

    if (request_queue.empty())
        return 0;

    Tick current_tick = request_queue.front().tick;
    Tick total_delay = 0;
    uint64_t success_request = 0;
    auto cache = BaselineCache([&](Request req) {
        printf("finish request %lld\n", req.id);
        success_request++;
        total_delay += current_tick - req.tick;
    });

    while (success_request < total_request) {
        if (cache.isAvailable() &&
            !request_queue.empty() &&
            request_queue.front().tick + 6 <= current_tick) {
            printf("send request %lld\n", request_queue.front().id);
            cache.requestCache(request_queue.front());
            request_queue.pop();
        }

        current_tick++;
        cache.nextTick(current_tick);
        if (current_tick % 100000 == 0)
            printf("current_tick: %lld\n", current_tick);
    }

    cout << total_delay << endl;

    return 0;
}
