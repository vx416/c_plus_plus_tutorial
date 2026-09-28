/**
 * 練習 2: Observer Events
 *
 * 實作簡單 EventBus，publish 時通知所有 subscriber。
 *
 * 重點練習：
 *   - event producer 不知道 observer 具體型別
 *   - std::function 作為 callback
 */

#include <cassert>
#include <functional>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>
using namespace std;

class EventBus {
public:
    using Handler = function<void(string_view)>;

    void subscribe(Handler handler) {
        handlers_.push_back(std::move(handler));
    }

    void publish(string_view event) const {
        for (const Handler& handler : handlers_) {
            handler(event);
        }
    }

private:
    vector<Handler> handlers_;
};

int main() {
    EventBus bus;
    vector<string> received;

    bus.subscribe([&](string_view event) {
        received.push_back(string(event));
    });
    bus.subscribe([&](string_view event) {
        received.push_back("log:" + string(event));
    });

    bus.publish("created");

    assert((received == vector<string>{"created", "log:created"}));
    cout << "ex02 passed!" << endl;
    return 0;
}
