/**
 * 練習 5: RAII Logger
 *
 * 實作一個 ScopeLogger class：
 *   - 建構時印出 "ENTER: <name>"
 *   - 解構時印出 "EXIT: <name>"
 *
 * 這是 RAII 最簡單的應用之一，用來追蹤函式的進入/離開。
 * 實務上 production code 常用類似的 scope guard 來做 profiling。
 */

#include <iostream>
#include <string>
#include <sstream>
#include <cassert>
using namespace std;

class ScopeLogger {
public:
    explicit ScopeLogger(const string& name): name_(name) {
        cout << "ENTER: " << name_ << endl;
    }

    ~ScopeLogger() {
        cout << "EXIT: " << name_ << endl;
    }


private:
    // TODO: 成員變數
    string name_;
};

// 用來捕捉 cout 輸出的 helper
string capture_output(void (*func)()) {
    stringstream buffer;
    streambuf* old = cout.rdbuf(buffer.rdbuf());
    func();
    cout.rdbuf(old);
    return buffer.str();
}

void test_basic() {
    ScopeLogger log("main");
}

void test_nested() {
    ScopeLogger outer("outer");
    {
        ScopeLogger inner("inner");
    }
}

int main() {
    string out1 = capture_output(test_basic);
    assert(out1.find("ENTER: main") != string::npos);
    assert(out1.find("EXIT: main") != string::npos);
    // ENTER 要在 EXIT 之前
    assert(out1.find("ENTER: main") < out1.find("EXIT: main"));

    string out2 = capture_output(test_nested);
    // 順序應該是: ENTER outer → ENTER inner → EXIT inner → EXIT outer
    size_t p1 = out2.find("ENTER: outer");
    size_t p2 = out2.find("ENTER: inner");
    size_t p3 = out2.find("EXIT: inner");
    size_t p4 = out2.find("EXIT: outer");
    assert(p1 < p2 && p2 < p3 && p3 < p4);

    cout << "ex05 passed!" << endl;
    return 0;
}
