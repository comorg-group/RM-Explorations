#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include "global.hh"
#include "BaseCache.hh"
#include "BaselineCache.hh"
#include "EagerCache.hh"
#include "MassiveCache.hh"

using namespace std;

int main(int argc, char** argv)
{
    bool analysis = false;
    if (argc > 2)
        analysis = strcmp(argv[2], "--analysis") == 0;
    fstream fs, verbfile;
    fs.open(argv[1]);
    verbfile.open("output", ios::out);
    if (!fs.is_open())
        return -1;

    uint64_t total_request = 0;
    uint64_t read_count = 0;
    uint64_t write_count = 0;
    vector<Request> request_vector;
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
        if (operation == OpRead)
            read_count++;
        else
            write_count++;
        request_vector.push_back({ operation, total_request, address, tick / 1000, 0, false });
    }

    if (request_vector.empty())
        return 0;

    if (analysis) {
        uint64_t total_interval = 0;
        map<Addr, int> address_visit_count;
        for (auto it = request_vector.begin(); it != request_vector.end() - 1; it++) {
            total_interval += (it + 1)->tick - it->tick;
            address_visit_count[it->address]++;
        }

        cout << "Total request:                 " << total_request << endl
             << "Read percentage:               " << (float)read_count / (float)total_request * 100 << "%" << endl
             << "Write percentage:              " << (float)write_count / (float)total_request * 100 << "%" << endl
             << "Average interval:              " << (float)total_interval / total_request << endl
             << "Average requests per address:  " << (float)total_request / address_visit_count.size() << endl;
        return 0;
    }

    Tick current_tick = request_vector.front().tick;
    Tick total_delay = 0;
    Tick total_shift_time = 0;
    Tick total_shift_time_in_read = 0;
    uint64_t success_request = 0;
    int total_miss = 0;
    int percent = 0; // 0..100
    auto cache = MassiveCache([&](Request req) {
        debug("@%lld: Finish request: request.id: %lld success_request: %lld", current_tick, req.id, success_request);
        success_request++;
#ifndef DEBUG
        if (success_request >= total_request * percent / 100) {
            if (percent % 10 == 0) {
                cout << percent << "%";
                if (percent == 100)
                    cout << endl;
            } else {
                cout << ".";
            }
            cout.flush();
            percent += 2;
        }
#endif
        auto delay = current_tick - req.tick;
        verbfile << delay << endl;
        total_delay += delay;
        total_shift_time += req.shiftTime;
        if (req.op == OpRead)
<<<<<<< HEAD
            total_shift_time += req.shiftTime;
        if (req.shiftTime>5) {
            _LIBCPP_ASSERT(1, 1);
        }
=======
            total_shift_time_in_read += req.shiftTime;
>>>>>>> d8c89f350449b309ce7a2f90918d25e3e6216995
        total_miss += req.isMissed;
    });

    uint64_t total_issue_delay = 0;
    auto it = request_vector.begin();
    while (success_request < total_request) {
        if (cache.isAvailable() && it != request_vector.end() && it->tick + 6 <= current_tick) {
            debug("@%lld: Send request: request.id: %lld / %lld", current_tick, it->id, total_request);
            total_issue_delay += current_tick - it->tick - 6;
            cache.requestCache(*it);
            it++;
        }
        current_tick++;
        cache.nextTick(current_tick);
    }

    cout << "Total request:                 " << total_request << endl
         << "Total delay:                   " << total_delay << endl
         << "Average delay:                 " << (float)total_delay / (float)total_request << endl
         << "Average delay before issue:    " << (float)total_issue_delay / (float)total_request << endl
         << "Average shift:                 " << (float)total_shift_time / (float)total_request << endl
         << "Average shift (when read):     " << (float)total_shift_time_in_read / (float)read_count << endl
         << "Miss rate:                     " << (float)total_miss / (float)total_request * 100 << "%" << endl
         << "Read percentage:               " << (float)read_count / (float)total_request * 100 << "%" << endl
         << "Write percentage:              " << (float)write_count / (float)total_request * 100 << "%" << endl
         ;

    return 0;
}
