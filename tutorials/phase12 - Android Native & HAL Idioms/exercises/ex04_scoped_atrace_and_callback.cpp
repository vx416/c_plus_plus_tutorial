/**
 * 練習 4: ScopedTrace (ATRACE_CALL) 與非同步結果排序器
 *
 * 1. 實作 ScopedTrace，在建構時寫入 "B|<tag>"，解構時寫入 "E|<tag>"。
 * 2. 即使函式提早 return，ScopedTrace 也必須確保 "E|<tag>" 被紀錄。
 */

#include <cassert>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>
using namespace std;

class ScopedTrace {
public:
    ScopedTrace(string_view tag, vector<string>* log)
        : tag_(tag), log_(log) {
        if (log_) {
            log_->push_back("B|" + tag_);
        }
    }

    ~ScopedTrace() {
        if (log_) {
            log_->push_back("E|" + tag_);
        }
    }

    ScopedTrace(const ScopedTrace&) = delete;
    ScopedTrace& operator=(const ScopedTrace&) = delete;

private:
    string tag_;
    vector<string>* log_;
};

bool ProcessFrameWithTrace(int frame_id, vector<string>* trace_log) {
    ScopedTrace outer("ProcessFrame", trace_log);
    if (frame_id < 0) {
        // 提早返回時，outer 的 destructor 仍會確保寫入 E|ProcessFrame
        return false;
    }
    {
        ScopedTrace inner("Run3A", trace_log);
    }
    return true;
}

int main() {
    vector<string> log;

    assert(!ProcessFrameWithTrace(-1, &log));
    assert(log.size() == 2);
    assert(log[0] == "B|ProcessFrame");
    assert(log[1] == "E|ProcessFrame");

    log.clear();
    assert(ProcessFrameWithTrace(10, &log));
    assert(log.size() == 4);
    assert(log[0] == "B|ProcessFrame");
    assert(log[1] == "B|Run3A");
    assert(log[2] == "E|Run3A");
    assert(log[3] == "E|ProcessFrame");

    cout << "ex04 passed!" << endl;
    return 0;
}
