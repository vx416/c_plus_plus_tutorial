/**
 * Phase 12-1: -fno-exceptions, StatusOr<T> & Sigil (Passkey) Factory Idiom
 *
 * 在 Android AOSP 與 Camera HAL (如 LyricHal) 中，編譯器預設帶上：
 *   -fno-exceptions -fno-rtti
 *
 * 這帶來兩個核心工程問題：
 *   1. 沒有 try/catch，錯誤怎麼傳遞才不會寫出滿山滿谷的 if (err != OK) return err？
 *      -> 解法：Status / StatusOr<T>（或 android::base::Result<T>）搭配
 *         RETURN_IF_ERROR 與 ASSIGN_OR_RETURN 巨集。
 *   2. Constructor 沒有回傳值，又不能 throw exception，那建構過程可能失敗（例如讀檔/開硬體）怎麼辦？
 *      - 反模式 (Anti-pattern)：寫一個兩階段 `bool Init()`，結果呼叫端常忘記呼叫 Init() 就直接用半成品物件。
 *      - 正解 (Sigil / Passkey Factory)：
 *        提供 `static StatusOr<unique_ptr<T>> Create(...)` 在建構前完成所有會失敗的檢查，
 *        並用 private `struct Sigil {}` 鎖住 constructor，讓外部無法直接呼叫 make_unique<T>()，
 *        確保「只要拿得到物件指標，它就 100% 是初始化成功的有效物件」！
 */

#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <variant>
using namespace std;

// ============================================================
// 1. Status 與 StatusOr<T>（對應 absl::Status / absl::StatusOr）
// ============================================================
enum class ErrorCode {
    kOk = 0,
    kInvalidArgument,
    kHardwareNotFound,
    kPermissionDenied,
};

class [[nodiscard]] Status {
public:
    Status() = default;
    Status(ErrorCode code, string message)
        : code_(code), message_(std::move(message)) {}

    static Status OkStatus() { return Status(); }

    bool ok() const { return code_ == ErrorCode::kOk; }
    ErrorCode code() const { return code_; }
    const string& message() const { return message_; }

private:
    ErrorCode code_ = ErrorCode::kOk;
    string message_;
};

template <typename T>
class [[nodiscard]] StatusOr {
public:
    // 成功：隱式從 T 建構
    StatusOr(T value) : data_(std::move(value)) {}

    // 失敗：隱式從非 OK 的 Status 建構
    StatusOr(Status status) : data_(std::move(status)) {}

    bool ok() const { return holds_alternative<T>(data_); }

    const Status& status() const {
        static const Status kOk = Status::OkStatus();
        return ok() ? kOk : get<Status>(data_);
    }

    T& value() & { return get<T>(data_); }
    const T& value() const& { return get<T>(data_); }
    T&& value() && { return std::move(get<T>(data_)); }

    T& operator*() & { return value(); }
    const T& operator*() const& { return value(); }
    T* operator->() { return &value(); }
    const T* operator->() const { return &value(); }

private:
    variant<Status, T> data_;
};

// 錯誤傳遞巨集（模擬 RETURN_IF_ERROR）
#define RETURN_IF_ERROR(expr)              \
    do {                                   \
        Status _status = (expr);           \
        if (!_status.ok()) return _status; \
    } while (0)

// ============================================================
// 2. Sigil (Passkey) Idiom + Static Factory
// ============================================================
// 本章重點：
//   為什麼不直接把 constructor 設成 private，然後在 Create() 裡 return unique_ptr<T>(new T(...))？
//   1. 使用 std::make_unique<T> 更一致且符合現代 C++ 規範。
//   2. 但 std::make_unique<T> 是外部 template 函式，無法呼叫 private constructor。
//   3. 解法：把 constructor 設為 public，但第一個參數要求傳入 private 的 `Sigil` 空結構！
//      外部拿不到 Sigil，所以無法偷呼叫 constructor；而 Create() 是成員函式，可以傳 Sigil{} 給 make_unique！
class ImageSensorDevice {
private:
    struct Sigil {};  // 只有 ImageSensorDevice 內部才看得到的通行證 (Passkey)

public:
    static StatusOr<unique_ptr<ImageSensorDevice>> Create(int sensor_id, int i2c_bus) {
        if (sensor_id < 0) {
            return Status(ErrorCode::kInvalidArgument, "sensor_id must be non-negative");
        }
        if (i2c_bus != 2 && i2c_bus != 4) {
            return Status(ErrorCode::kHardwareNotFound, "unsupported i2c_bus");
        }
        // 所有會失敗的驗證與資源準備都成功後，才透過 Sigil 建立不可變的有效物件
        return make_unique<ImageSensorDevice>(Sigil{}, sensor_id, i2c_bus);
    }

    // 雖然是 public（讓 make_unique 可以呼叫），但外部無法構造 private Sigil！
    ImageSensorDevice(Sigil, int sensor_id, int i2c_bus)
        : sensor_id_(sensor_id), i2c_bus_(i2c_bus) {}

    Status StreamOn(int fps) {
        if (fps <= 0 || fps > 240) {
            return Status(ErrorCode::kInvalidArgument, "invalid fps");
        }
        fps_ = fps;
        return Status::OkStatus();
    }

    int sensor_id() const { return sensor_id_; }
    int i2c_bus() const { return i2c_bus_; }
    int fps() const { return fps_; }

private:
    int sensor_id_;
    int i2c_bus_;
    int fps_ = 0;
};

Status ConfigureAndStartSensor(int sensor_id, int i2c_bus, int fps) {
    auto sensor_or = ImageSensorDevice::Create(sensor_id, i2c_bus);
    if (!sensor_or.ok()) {
        return sensor_or.status();
    }
    unique_ptr<ImageSensorDevice> sensor = std::move(*sensor_or);

    RETURN_IF_ERROR(sensor->StreamOn(fps));

    cout << "  [OK] sensor=" << sensor->sensor_id()
         << " bus=" << sensor->i2c_bus()
         << " streaming at " << sensor->fps() << " fps" << endl;
    return Status::OkStatus();
}

int main() {
    // Output:
    // === -fno-exceptions + StatusOr + Sigil Factory ===
    //   [OK] sensor=0 bus=2 streaming at 60 fps
    //   [Error] unsupported i2c_bus
    //   [Error] invalid fps
    //
    cout << "=== -fno-exceptions + StatusOr + Sigil Factory ===" << endl;

    Status s1 = ConfigureAndStartSensor(0, 2, 60);
    if (!s1.ok()) {
        cout << "  [Error] " << s1.message() << endl;
    }

    Status s2 = ConfigureAndStartSensor(0, 99, 60);
    if (!s2.ok()) {
        cout << "  [Error] " << s2.message() << endl;
    }

    Status s3 = ConfigureAndStartSensor(1, 4, 999);
    if (!s3.ok()) {
        cout << "  [Error] " << s3.message() << endl;
    }

    cout << endl;
    return 0;
}
