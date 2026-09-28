/**
 * 練習 2: String Buffer (Rule of Five)
 *
 * 實作一個管理動態 char 陣列的 class，完整支援 Rule of Five：
 *   - Constructor(const char*)
 *   - Destructor
 *   - Copy constructor（深拷貝）
 *   - Copy assignment
 *   - Move constructor
 *   - Move assignment
 *   - size(), c_str()
 *
 * 重點練習：手動管理資源、深拷貝 vs 移動、self-assignment 檢查
 */

#include <cstring>
#include <utility>
#include <cassert>
#include <iostream>
using namespace std;

class StringBuffer {
public:
    // TODO: Constructor，從 const char* 複製字串
    explicit StringBuffer(const char* s) {
        if (s == nullptr) return;

        size_ = strlen(s);
        data_ = new char[size_ + 1];
        strcpy(data_, s);
    }

    // TODO: Destructor，釋放記憶體
    ~StringBuffer() {
        delete[] data_;
    }

    // TODO: Copy constructor，深拷貝
    StringBuffer(const StringBuffer& other) : size_(other.size_) {
        if (other.data_ == nullptr) return;

        data_ = new char[size_ + 1];
        strcpy(data_, other.data_);
    }

    // TODO: Copy assignment，記得處理 self-assignment
    StringBuffer& operator=(const StringBuffer& other) {
        if (this == &other) return *this;

        char* new_data = nullptr;
        if (other.data_ != nullptr) {
            new_data = new char[other.size_ + 1];
            strcpy(new_data, other.data_);
        }

        delete[] data_;
        data_ = new_data;
        size_ = other.size_;
        return *this;
    }

    // TODO: Move constructor，用 noexcept
    StringBuffer(StringBuffer&& other) noexcept
        : data_(other.data_), size_(other.size_) {
        other.data_ = nullptr;
        other.size_ = 0;
    }

    // TODO: Move assignment
    StringBuffer& operator=(StringBuffer&& other) noexcept {
        if (this == &other) return *this;

        delete[] data_;
        data_ = other.data_;
        size_ = other.size_;

        other.data_ = nullptr;
        other.size_ = 0;
        return *this;
    }

    size_t size() const { return size_; }
    const char* c_str() const { return data_; }

private:
    char* data_ = nullptr;
    size_t size_ = 0;
};

int main() {
    // --- 基本建構 ---
    StringBuffer s1("hello");
    assert(s1.size() == 5);
    assert(strcmp(s1.c_str(), "hello") == 0);

    // --- Copy ---
    StringBuffer s2 = s1;
    assert(strcmp(s2.c_str(), "hello") == 0);
    assert(s1.c_str() != s2.c_str());  // 深拷貝，位址不同

    // --- Copy assignment ---
    StringBuffer s3("world");
    s3 = s1;
    assert(strcmp(s3.c_str(), "hello") == 0);

    // self-assignment
    s3 = s3;
    assert(strcmp(s3.c_str(), "hello") == 0);

    // --- Move ---
    StringBuffer s4 = std::move(s1);
    assert(strcmp(s4.c_str(), "hello") == 0);
    assert(s1.size() == 0);  // 被掏空

    // --- Move assignment ---
    StringBuffer s5("temp");
    s5 = std::move(s2);
    assert(strcmp(s5.c_str(), "hello") == 0);

    cout << "ex02 passed!" << endl;
    cout << "（建議用 -fsanitize=address 編譯確認沒有 memory leak）" << endl;
    return 0;
}
