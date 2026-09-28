/**
 * 練習 1: Move-only Buffer
 *
 * 實作一個只能 move、不能 copy 的 Buffer。
 *
 * 重點練習：
 *   - delete copy ctor / copy assignment
 *   - 實作 move ctor / move assignment
 *   - move 後來源物件要保持可解構狀態
 */

#include <cassert>
#include <iostream>
#include <memory>
#include <utility>
using namespace std;

class Buffer {
public:
    explicit Buffer(size_t size) : size_(size), data_(make_unique<int[]>(size)) {}

    Buffer(const Buffer&) = delete;
    Buffer& operator=(const Buffer&) = delete;

    Buffer(Buffer&& other) noexcept
        : size_(other.size_), data_(std::move(other.data_)) {
        // TODO: move 後，other 仍要能安全解構。
        other.size_ = 0;
    }

    Buffer& operator=(Buffer&& other) noexcept {
        // TODO: 釋放自己原本資源，接收 other 資源，並清空 other 狀態。
        if (this == &other) {
            return *this;
        }
        size_ = other.size_;
        data_ = std::move(other.data_);
        other.size_ = 0;
        return *this;
    }

    size_t size() const { return size_; }
    bool empty() const { return size_ == 0; }

private:
    size_t size_ = 0;
    unique_ptr<int[]> data_;
};

int main() {
    Buffer a(10);
    assert(a.size() == 10);

    Buffer b(std::move(a));
    assert(b.size() == 10);
    assert(a.empty());

    Buffer c(1);
    c = std::move(b);
    assert(c.size() == 10);
    assert(b.empty());

    cout << "ex01 passed!" << endl;
    return 0;
}
