/**
 * 練習 5: Variant Message
 *
 * 用 variant 表示不同訊息型別，並把它們格式化成字串。
 *
 * 重點練習：
 *   - variant 表示「幾種已知型別中選一種」
 *   - visit 根據目前型別呼叫對應處理
 */

#include <cassert>
#include <iostream>
#include <string>
#include <variant>
#include <vector>
using namespace std;

struct Text {
    string body;
};

struct Join {
    string user;
};

struct Leave {
    string user;
};

using Event = variant<Text, Join, Leave>;

struct Formatter {
    string operator()(const Text& event) const {
        return "text:" + event.body;
    }

    string operator()(const Join& event) const {
        return "join:" + event.user;
    }

    string operator()(const Leave& event) const {
        return "leave:" + event.user;
    }
};

string format_event(const Event& event) {
    // TODO: 用 visit 處理目前 variant 裝的是哪個型別。
    return visit(Formatter{}, event);
}

int main() {
    vector<Event> events{Join{"Alice"}, Text{"hello"}, Leave{"Alice"}};

    assert(format_event(events[0]) == "join:Alice");
    assert(format_event(events[1]) == "text:hello");
    assert(format_event(events[2]) == "leave:Alice");

    cout << "ex05 passed!" << endl;
    return 0;
}
