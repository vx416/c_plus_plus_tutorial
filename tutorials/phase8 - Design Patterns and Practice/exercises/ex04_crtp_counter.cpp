/**
 * 練習 4: CRTP Counter
 *
 * 用 CRTP mixin 提供 instance counting。
 *
 * 重點練習：
 *   - Base<Derived> 知道 Derived 型別
 *   - 每個 Derived 有自己的 static counter
 */

#include <cassert>
#include <iostream>
using namespace std;

template <typename Derived>
class InstanceCounter {
public:
    InstanceCounter() {
        ++count_;
    }

    InstanceCounter(const InstanceCounter&) {
        ++count_;
    }

    ~InstanceCounter() {
        --count_;
    }

    static int count() {
        return count_;
    }

private:
    static inline int count_ = 0;
};

class User : public InstanceCounter<User> {};
class Order : public InstanceCounter<Order> {};

int main() {
    assert(User::count() == 0);
    {
        User a;
        User b;
        Order order;
        assert(User::count() == 2);
        assert(Order::count() == 1);
    }
    assert(User::count() == 0);
    assert(Order::count() == 0);

    cout << "ex04 passed!" << endl;
    return 0;
}
