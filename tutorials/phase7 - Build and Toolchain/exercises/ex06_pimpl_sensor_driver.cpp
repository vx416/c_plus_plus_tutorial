/**
 * 練習 6: Pimpl Idiom 封裝
 *
 * 將 SensorCalibration 的內部資料結構封裝進 `struct Impl`，
 * 使得：
 *   1. sizeof(SensorCalibration) 等於 sizeof(void*)
 *   2. 支援 AddPoint(int temp_c, int offset) 與 LookupOffset(int temp_c)
 *   3. 支援 move 建構與移動賦值
 */

#include <cassert>
#include <iostream>
#include <map>
#include <memory>
using namespace std;

class SensorCalibration {
public:
    SensorCalibration();
    ~SensorCalibration();

    SensorCalibration(SensorCalibration&&) noexcept;
    SensorCalibration& operator=(SensorCalibration&&) noexcept;

    void AddPoint(int temp_c, int offset);
    int LookupOffset(int temp_c, int default_offset = 0) const;
    size_t PointCount() const;

private:
    struct Impl;
    unique_ptr<Impl> impl_;
};

struct SensorCalibration::Impl {
    map<int, int> temp_to_offset;
};

SensorCalibration::SensorCalibration() : impl_(make_unique<Impl>()) {}
SensorCalibration::~SensorCalibration() = default;
SensorCalibration::SensorCalibration(SensorCalibration&&) noexcept = default;
SensorCalibration& SensorCalibration::operator=(SensorCalibration&&) noexcept = default;

void SensorCalibration::AddPoint(int temp_c, int offset) {
    impl_->temp_to_offset[temp_c] = offset;
}

int SensorCalibration::LookupOffset(int temp_c, int default_offset) const {
    auto it = impl_->temp_to_offset.find(temp_c);
    if (it == impl_->temp_to_offset.end()) {
        return default_offset;
    }
    return it->second;
}

size_t SensorCalibration::PointCount() const {
    return impl_->temp_to_offset.size();
}

int main() {
    static_assert(sizeof(SensorCalibration) == sizeof(void*),
                  "Pimpl class should only hold a single pointer");

    SensorCalibration cal;
    cal.AddPoint(25, -2);
    cal.AddPoint(50, 4);

    assert(cal.PointCount() == 2);
    assert(cal.LookupOffset(25) == -2);
    assert(cal.LookupOffset(50) == 4);
    assert(cal.LookupOffset(80, 99) == 99);

    // 測試 move semantics
    SensorCalibration moved_cal = std::move(cal);
    assert(moved_cal.PointCount() == 2);
    assert(moved_cal.LookupOffset(50) == 4);

    cout << "ex06 passed!" << endl;
    return 0;
}
