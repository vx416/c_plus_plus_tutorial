/**
 * 練習 1: Sigil Passkey Factory + StatusOr<T> (在 -fno-exceptions 下編譯)
 *
 * 請完成 SensorModeTable：
 *   - 使用 private `struct Sigil {}` 保護 constructor
 *   - 實作 `static StatusOr<unique_ptr<SensorModeTable>> Create(span<const int> fps_list)`：
 *     1. 若 fps_list 為空，回傳失敗 Status("empty mode list")
 *     2. 若任一 fps <= 0，回傳失敗 Status("fps must be positive")
 *     3. 驗證通過時，回傳 make_unique<SensorModeTable>(Sigil{}, ...)
 */

#include <cassert>
#include <iostream>
#include <memory>
#include <span>
#include <string>
#include <utility>
#include <variant>
#include <vector>
using namespace std;

class [[nodiscard]] Status {
public:
    Status() = default;
    explicit Status(string error_msg) : ok_(false), msg_(std::move(error_msg)) {}

    static Status OkStatus() { return Status(); }
    bool ok() const { return ok_; }
    const string& message() const { return msg_; }

private:
    bool ok_ = true;
    string msg_;
};

template <typename T>
class [[nodiscard]] StatusOr {
public:
    StatusOr(T value) : data_(std::move(value)) {}
    StatusOr(Status status) : data_(std::move(status)) {}

    bool ok() const { return holds_alternative<T>(data_); }
    const Status& status() const {
        static const Status kOk = Status::OkStatus();
        return ok() ? kOk : get<Status>(data_);
    }
    T& value() & { return get<T>(data_); }
    T&& value() && { return std::move(get<T>(data_)); }
    T& operator*() & { return value(); }
    T* operator->() { return &value(); }

private:
    variant<Status, T> data_;
};

class SensorModeTable {
private:
    struct Sigil {};

public:
    static StatusOr<unique_ptr<SensorModeTable>> Create(span<const int> fps_list) {
        if (fps_list.empty()) {
            return Status("empty mode list");
        }
        for (int fps : fps_list) {
            if (fps <= 0) {
                return Status("fps must be positive");
            }
        }
        vector<int> modes(fps_list.begin(), fps_list.end());
        return make_unique<SensorModeTable>(Sigil{}, std::move(modes));
    }

    SensorModeTable(Sigil, vector<int> modes) : modes_(std::move(modes)) {}

    size_t count() const { return modes_.size(); }
    int max_fps() const {
        int best = modes_[0];
        for (int f : modes_) {
            if (f > best) best = f;
        }
        return best;
    }

private:
    vector<int> modes_;
};

int main() {
    vector<int> empty_list;
    auto r1 = SensorModeTable::Create(empty_list);
    assert(!r1.ok());
    assert(r1.status().message() == "empty mode list");

    int bad_list[] = {30, 60, -15, 120};
    auto r2 = SensorModeTable::Create(bad_list);
    assert(!r2.ok());
    assert(r2.status().message() == "fps must be positive");

    int valid_list[] = {24, 30, 60, 120};
    auto r3 = SensorModeTable::Create(valid_list);
    assert(r3.ok());
    assert((*r3)->count() == 4);
    assert((*r3)->max_fps() == 120);

    cout << "ex01 passed!" << endl;
    return 0;
}
