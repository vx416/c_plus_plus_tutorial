/**
 * 練習 2: Fixed Stack Template
 *
 * 實作固定容量 Stack<T, N>：
 *   - push
 *   - pop
 *   - top
 *   - empty / full / size
 *
 * 重點練習：class template、non-type template parameter、例外處理
 */

#include <array>
#include <cassert>
#include <iostream>
#include <stdexcept>
#include <string>
using namespace std;

template <typename T, size_t N>
class Stack {
public:
    void push(const T& value) {
        // TODO: 滿了要丟出 overflow_error
        // 這個版本用 const T&，所以 push(lvalue) 不會先複製參數。
        // 若要支援更高效的 rvalue push，可以再加 overload:
        //   void push(T&& value) { data_[size_++] = std::move(value); }
        if (full()) {
            throw overflow_error("stack is full");
        }
        data_[size_++] = value;
    }

    T pop() {
        // TODO: 空了要丟出 underflow_error
        // pop 回傳 T by value，因為元素離開 stack 後，不能回傳內部 reference。
        // 對 string/vector 來說，實務上可用 std::move(data_[--size_]) 減少複製；
        // 這裡先保留簡單寫法，重點放在 template 和容量 N。
        if (empty()) {
            throw underflow_error("stack is empty");
        }
        return data_[--size_];
    }

    const T& top() const {
        // TODO: 回傳頂端元素
        // top 只觀察頂端元素，不移除它，所以可以安全回傳 const reference。
        if (empty()) {
            throw underflow_error("stack is empty");
        }
        return data_[size_ - 1];
    }

    bool empty() const { return size_ == 0; }
    bool full() const { return size_ == N; }
    size_t size() const { return size_; }

private:
    // N 是 class 的一部分：Stack<int, 3> 和 Stack<int, 10> 是不同型別。
    // array<T, N> 會把容量放在型別裡，因此不需要 runtime 動態配置。
    array<T, N> data_{};
    size_t size_ = 0;
};

int main() {
    Stack<int, 3> nums;
    assert(nums.empty());
    assert(!nums.full());

    nums.push(10);
    nums.push(20);
    assert(nums.size() == 2);
    assert(nums.top() == 20);

    nums.push(30);
    assert(nums.full());
    assert(nums.pop() == 30);
    assert(nums.pop() == 20);
    assert(nums.pop() == 10);
    assert(nums.empty());

    bool underflow = false;
    try {
        nums.pop();
    } catch (const underflow_error&) {
        underflow = true;
    }
    assert(underflow);

    Stack<string, 2> words;
    words.push("hello");
    words.push("template");
    assert(words.top() == "template");

    bool overflow = false;
    try {
        words.push("boom");
    } catch (const overflow_error&) {
        overflow = true;
    }
    assert(overflow);

    cout << "ex02 passed!" << endl;
    return 0;
}
