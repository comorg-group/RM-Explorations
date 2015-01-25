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
        string op;
        Addr address;
        int useless;
        Tick tick;

        fs >> op >> address >> useless >> useless >> tick;
        if (op != "r" && op != "w" && op != "u")
            continue;
        total_request++;
        Operation operation = (op == "r") ? OpRead : ((op == "w") ? OpWrite : OpUpdate);
        request_queue.push({ operation, total_request, address, tick / 1000, 0, false });
    }

    if (request_queue.empty())
        return 0;

    Tick current_tick = request_queue.front().tick;
    Tick total_delay = 0;
    Tick total_shift_time = 0;
    int total_miss = 0;
    int read_count = 0;
    int write_count = 0;
    uint64_t success_request = 0;
    auto cache = BaselineCache([&](Request req) {
        debug("@%lld: Finish request: request.id: %lld success_request: %lld", current_tick, req.id, success_request);
        success_request++;
        auto delay = current_tick - req.tick;
        verbfile << delay << endl;
        total_delay += delay;
        if (req.op == OpRead) {
            total_shift_time += req.shiftTime;
            read_count++;
        } else {
            write_count++;
        }
        total_miss += req.isMissed;
    });

    while (success_request < total_request) {
        if (cache.isAvailable() && !request_queue.empty() && request_queue.front().tick + 6 <= current_tick) {
            debug("@%lld: Send request: request.id: %lld / %lld", current_tick, request_queue.front().id, total_request);
            cache.requestCache(request_queue.front());
            request_queue.pop();
        }

        current_tick++;
        cache.nextTick(current_tick);
    }

    cout << "Total request: " << total_request << endl
         << "Total delay: " << total_delay << endl
         << "Total shift (when read): " << total_shift_time << endl
         << "Average delay: " << (float)total_delay / (float)total_request << endl
         << "Average shift (when read): " << (float)total_shift_time / (float)read_count << endl
         << "Miss rate: " << (float)total_miss / (float)total_request << endl
         << "Read percentage: " << (float)read_count / (float)total_request << endl
         << "Write percentage: " << (float)write_count / (float)total_request << endl
         ;

    return 0;
}
