#include <iostream>
#include <fstream>
#include <queue>
#include "global.hh"
#include "BaseCache.hh"
#include "BaselineCache.hh"

using namespace std;

int main(int argc, char** argv)
{
    fstream fs, verbfile;
    fs.open(argv[1]);
    verbfile.open("output", ios::out);
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
        if (op != "r" && op != "w" && op != "u")
            continue;
        Operation operation = (op == "r") ? OpRead : ((op == "w") ? OpWrite : OpUpdate);
        request_queue.push({ operation, total_request, address, tick / 1000 });
    }

    if (request_queue.empty())
        return 0;

    Tick current_tick = request_queue.front().tick;
    Tick total_delay = 0;
    uint64_t success_request = 0;
    auto cache = BaselineCache([&](Request req) {
        debug("@%lld: Finish request: request.id: %lld success_request: %lld", current_tick, req.id, success_request);
        success_request++;
        auto delay = current_tick - req.tick;
        verbfile << delay << endl;
        total_delay += delay;
    });

    while (success_request < total_request) {
        if (cache.isAvailable() && !request_queue.empty() && request_queue.front().tick + 6 <= current_tick) {
            debug("@%lld: Send request: request.id: %lld", current_tick, request_queue.front().id);
            cache.requestCache(request_queue.front());
            request_queue.pop();
        }

        current_tick++;
        cache.nextTick(current_tick);
    }

    cout << "Total request: " << total_request << endl
         << "Total delay: " << total_delay << endl
         << "Average delay: " << total_delay / total_request << endl;

    return 0;
}
