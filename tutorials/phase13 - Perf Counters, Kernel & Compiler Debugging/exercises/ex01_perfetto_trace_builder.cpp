/**
 * 練習 1: Perfetto `trace_marker` 字串產生器與 Async Slice 配對檢查
 *
 * 1. 實作 FormatAsyncBegin(pid, name, cookie) -> "S|<pid>|<name>|<cookie>"
 * 2. 實作 FormatAsyncEnd(pid, name, cookie)   -> "F|<pid>|<name>|<cookie>"
 * 3. 實作 FormatCounter(pid, name, value)     -> "C|<pid>|<name>|<value>"
 * 4. 實作 HasUnfinishedAsyncSlices(events)：檢查是否每個 "S|pid|name|cookie"
 *    都有對應的 "F|pid|name|cookie"，若有未關閉的 async slice 回傳 true。
 */

#include <cassert>
#include <cstdint>
#include <iostream>
#include <set>
#include <string>
#include <string_view>
#include <vector>
using namespace std;

string FormatAsyncBegin(int pid, string_view name, int64_t cookie) {
    return "S|" + to_string(pid) + "|" + string(name) + "|" + to_string(cookie);
}

string FormatAsyncEnd(int pid, string_view name, int64_t cookie) {
    return "F|" + to_string(pid) + "|" + string(name) + "|" + to_string(cookie);
}

string FormatCounter(int pid, string_view name, int64_t value) {
    return "C|" + to_string(pid) + "|" + string(name) + "|" + to_string(value);
}

bool HasUnfinishedAsyncSlices(const vector<string>& events) {
    set<string> in_flight;
    for (const string& ev : events) {
        if (ev.rfind("S|", 0) == 0) {
            in_flight.insert(ev.substr(2));  // 存入 "<pid>|<name>|<cookie>"
        } else if (ev.rfind("F|", 0) == 0) {
            in_flight.erase(ev.substr(2));
        }
    }
    return !in_flight.empty();
}

int main() {
    assert(FormatAsyncBegin(900, "CaptureFrame", 42) == "S|900|CaptureFrame|42");
    assert(FormatAsyncEnd(900, "CaptureFrame", 42) == "F|900|CaptureFrame|42");
    assert(FormatCounter(900, "FreeBuffers", 7) == "C|900|FreeBuffers|7");

    vector<string> balanced{
        FormatAsyncBegin(900, "CaptureFrame", 1),
        FormatAsyncBegin(900, "CaptureFrame", 2),
        FormatCounter(900, "InFlight", 2),
        FormatAsyncEnd(900, "CaptureFrame", 1),
        FormatAsyncEnd(900, "CaptureFrame", 2),
        FormatCounter(900, "InFlight", 0),
    };
    assert(!HasUnfinishedAsyncSlices(balanced));

    vector<string> leaked{
        FormatAsyncBegin(900, "CaptureFrame", 1),
        FormatAsyncBegin(900, "CaptureFrame", 2),
        FormatAsyncEnd(900, "CaptureFrame", 1),
        // Frame 2 dropped without F| event!
    };
    assert(HasUnfinishedAsyncSlices(leaked));

    cout << "ex01 passed!" << endl;
    return 0;
}
