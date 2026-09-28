/**
 * 練習 3: Type-erased Command
 *
 * 用 std::function 儲存不同型別的 command。
 *
 * 重點練習：
 *   - 呼叫端只知道 void() 這個操作
 *   - 具體 command 可以是 lambda 或 functor
 */

#include <cassert>
#include <functional>
#include <iostream>
#include <vector>
using namespace std;

class CommandQueue {
public:
    void add(function<void()> command) {
        commands_.push_back(std::move(command));
    }

    void run_all() const {
        for (const auto& command : commands_) {
            command();
        }
    }

private:
    vector<function<void()>> commands_;
};

int main() {
    int value = 0;
    CommandQueue queue;
    queue.add([&] { value += 1; });
    queue.add([&] { value *= 10; });
    queue.run_all();

    assert(value == 10);
    cout << "ex03 passed!" << endl;
    return 0;
}
