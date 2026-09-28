/**
 * 練習 5: Observer Pattern (迷你版)
 *
 * 實作一個簡單的觀察者模式：
 *   - Observer (abstract): pure virtual on_event(const string& msg)
 *   - Subject: subscribe(Observer*), notify(const string& msg)
 *
 * Subject 維護一個 observer list（用 vector<Observer*>），
 * notify() 時呼叫每個 observer 的 on_event。
 *
 * 重點練習：
 *   - abstract class / pure virtual
 *   - 透過 base pointer 呼叫多型方法
 *   - virtual destructor
 */

#include <vector>
#include <string>
#include <cassert>
#include <iostream>
using namespace std;

class Observer {
public:
    // TODO: virtual destructor
    virtual ~Observer() = default;

    // TODO: pure virtual on_event(const string&)
    virtual void on_event(const string& msg) = 0;
};

class Subject {
public:
    // TODO: subscribe(Observer* o)：加入 list
    void subscribe(Observer* o) {
        observers_.push_back(o);
    }

    // TODO: notify(const string& msg)：呼叫所有 observer 的 on_event
    void notify(const string& msg) {
        for (Observer* observer : observers_) {
            observer->on_event(msg);
        }
    }

private:
    // TODO: 成員：vector<Observer*>
    vector<Observer*> observers_;
};

// 測試用的具體 observer
class Logger : public Observer {
public:
    void on_event(const string& msg) override {
        log_ += msg + ";";
    }
    string log() const { return log_; }
private:
    string log_;
};

class Counter : public Observer {
public:
    void on_event(const string& /*msg*/) override {
        ++count_;
    }
    int count() const { return count_; }
private:
    int count_ = 0;
};

int main() {
    Subject s;
    Logger logger;
    Counter counter;

    s.subscribe(&logger);
    s.subscribe(&counter);

    s.notify("event1");
    s.notify("event2");
    s.notify("event3");

    assert(logger.log() == "event1;event2;event3;");
    assert(counter.count() == 3);

    cout << "ex05 passed!" << endl;
    return 0;
}
