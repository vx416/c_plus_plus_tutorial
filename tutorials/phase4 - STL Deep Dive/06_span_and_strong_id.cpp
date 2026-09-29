/**
 * Phase 4-6: std::span & StrongId (Non-owning Buffer Views & Strong Types)
 *
 * 在 Android HAL（如 Camera HAL / Sensor driver）與 Google C++ 中，
 * 最常犯的兩類 API 設計問題是：
 *   1. 函式參數寫 const vector<uint8_t>&，迫使呼叫端如果手上只有 std::array、
 *      C array 或某段 buffer 的後半段，就必須額外配置記憶體複製一份 vector。
 *      -> 解法：C++20 std::span<const T>（或 absl::Span<const T>）
 *   2. 函式簽名寫 void Submit(int sensor_id, int frame_number, int request_id)，
 *      呼叫端不小心把順序傳反，編譯器完全不會報錯。
 *      -> 解法：StrongId / StrongIndex 強型別包裝
 *
 * 目錄:
 *   1. std::span<const T> 唯讀連續記憶體視圖
 *   2. std::span<T> 原地修改與 subspan 切片
 *   3. std::span 的生命週期陷阱
 *   4. StrongId / StrongIndex 強型別包裝
 */

#include <array>
#include <cstdint>
#include <iostream>
#include <numeric>
#include <span>
#include <vector>
using namespace std;

// ============================================================
// 1. std::span<const T> 唯讀連續記憶體視圖
// ============================================================
// 本章重點：
//   string_view 是「不擁有字串的唯讀視圖」；
//   std::span<const T> 則是「不擁有連續陣列 (vector / std::array / T[]) 的視圖」。
//   它內部只存 (T* ptr, size_t size)，傳遞成本極低，且不綁定特定容器型別。
uint32_t checksum_bytes(span<const uint8_t> buffer) {
    uint32_t sum = 0;
    for (uint8_t b : buffer) {
        sum += b;
    }
    return sum;
}

void span_readonly_demo() {
    // Output:
    // === std::span<const T> ===
    //   from vector:     60
    //   from std::array: 100
    //   from C array:    15
    //
    cout << "=== std::span<const T> ===" << endl;

    vector<uint8_t> vec{10, 20, 30};
    array<uint8_t, 4> arr{10, 20, 30, 40};
    uint8_t c_arr[] = {1, 2, 3, 4, 5};

    cout << "  from vector:     " << checksum_bytes(vec) << endl;
    cout << "  from std::array: " << checksum_bytes(arr) << endl;
    cout << "  from C array:    " << checksum_bytes(c_arr) << endl;
    cout << endl;
}

// ============================================================
// 2. std::span<T> 原地修改與 subspan 切片
// ============================================================
// 本章重點：
//   與 string_view 只能唯讀不同，span<T>（非 const）允許直接修改底層元素。
//   且可用 first(n)、last(n)、subspan(offset, count) 零拷貝切出子區間
//   （例如跳過影像 Frame Header 只把 Payload 傳給演算法）。
void apply_gain(span<uint16_t> pixels, uint16_t multiplier) {
    for (uint16_t& px : pixels) {
        px *= multiplier;
    }
}

void span_slice_demo() {
    // Output:
    // === std::span slice & mutate ===
    //   header:  999 888
    //   payload: 20 40 60 80
    //
    cout << "=== std::span slice & mutate ===" << endl;

    // 前 2 個元素是 header，後 4 個是 pixel payload
    vector<uint16_t> raw_frame{999, 888, 10, 20, 30, 40};

    span<uint16_t> full_view(raw_frame);
    span<const uint16_t> header = full_view.first(2);
    span<uint16_t> payload = full_view.subspan(2);  // 從 index 2 到結尾

    // 只放大 payload，不動到 header，而且完全沒有複製記憶體
    apply_gain(payload, 2);

    cout << "  header:  " << header[0] << " " << header[1] << endl;
    cout << "  payload:";
    for (uint16_t px : payload) {
        cout << ' ' << px;
    }
    cout << endl << endl;
}

// ============================================================
// 3. std::span 的生命週期陷阱
// ============================================================
// 本章重點：
//   span 跟 string_view 一樣不擁有記憶體：
//     1. 絕對不能回傳指向 local vector/array 的 span（dangling span）。
//     2. 如果底層 vector push_back 觸發重新配置，原本指向它的 span 立即失效。
void span_lifetime_demo() {
    // Output:
    // === std::span lifetime rules ===
    //   1. never return span to a local container
    //   2. vector reallocation invalidates existing spans
    //
    cout << "=== std::span lifetime rules ===" << endl;
    cout << "  1. never return span to a local container" << endl;
    cout << "  2. vector reallocation invalidates existing spans" << endl;
    cout << endl;
}

// ============================================================
// 4. StrongId / StrongIndex 強型別包裝
// ============================================================
// 本章重點：
//   using SensorId = int; 只是型別別名（type alias），編譯器仍把 SensorId 和 RequestId 當同一個 int。
//   若使用空的 Tag struct 搭配 template StrongId<Tag, T>：
//     - SensorId 與 RequestId 在編譯期是完全不相容的型別
//     - explicit constructor 禁止隱式從裸 int 轉換
//     - 執行期跟單一整數完全一樣大小（零額外記憶體開銷）
template <typename Tag, typename ValueType = uint32_t>
class StrongId {
public:
    constexpr explicit StrongId(ValueType value) : value_(value) {}

    constexpr ValueType value() const { return value_; }

    friend constexpr bool operator==(StrongId lhs, StrongId rhs) {
        return lhs.value_ == rhs.value_;
    }
    friend constexpr bool operator!=(StrongId lhs, StrongId rhs) {
        return lhs.value_ != rhs.value_;
    }
    friend constexpr bool operator<(StrongId lhs, StrongId rhs) {
        return lhs.value_ < rhs.value_;
    }

private:
    ValueType value_;
};

struct SensorIdTag {};
struct RequestIdTag {};
struct FrameNumberTag {};

using SensorId = StrongId<SensorIdTag, uint32_t>;
using RequestId = StrongId<RequestIdTag, uint32_t>;
using FrameNumber = StrongId<FrameNumberTag, uint64_t>;

void submit_capture(SensorId sensor_id, RequestId req_id, FrameNumber frame_num) {
    cout << "  submit_capture: sensor=" << sensor_id.value()
         << ", req=" << req_id.value()
         << ", frame=" << frame_num.value() << endl;
}

void strong_id_demo() {
    // Output:
    // === StrongId ===
    //   sizeof(SensorId) = 4
    //   submit_capture: sensor=0, req=42, frame=1001
    //
    cout << "=== StrongId ===" << endl;
    cout << "  sizeof(SensorId) = " << sizeof(SensorId) << endl;

    SensorId wide_sensor(0);
    RequestId req(42);
    FrameNumber frame(1001);

    submit_capture(wide_sensor, req, frame);
    // submit_capture(req, wide_sensor, frame); // 編譯錯誤！防止參數順序傳反
    // submit_capture(0, 42, 1001);             // 編譯錯誤！explicit 禁止裸數字隱式轉換
    cout << endl;
}

int main() {
    span_readonly_demo();
    span_slice_demo();
    span_lifetime_demo();
    strong_id_demo();
    return 0;
}
