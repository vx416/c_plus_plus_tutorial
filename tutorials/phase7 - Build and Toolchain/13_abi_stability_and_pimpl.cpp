/**
 * Phase 7-13: ABI Stability, Pimpl Idiom & Android Soong (Android.bp)
 *
 * 在大型系統（如 Android OS Framework 與 Vendor HAL 分離的 Project Treble / VNDK）中，
 * 常聽到「API 相容但 ABI 壞掉了 (ABI Breakage)」。
 *
 * 目錄:
 *   1. API vs ABI（為什麼改 private 成員也會弄壞 .so？）
 *   2. Pimpl (Pointer to Implementation) 隱藏實作與穩定 ABI
 *   3. 跨 .so 邊界的安全型別準則
 *   4. CMake vs Android Soong (Android.bp) 對照
 */

#include <iostream>
#include <memory>
#include <string>
#include <vector>
using namespace std;

// ============================================================
// 1. API vs ABI
// ============================================================
// 本章重點：
//   - API (Source compatibility)：重新編譯 caller 能不能過？
//   - ABI (Binary compatibility)：不重新編譯 caller，只替換 libfoo.so 會不會炸？
//
//   常見造成 C++ ABI 破壞的改動：
//     1. 在公開 class 新增/刪除 private 成員變數 -> sizeof(Class) 改變，caller 配置的記憶體大小錯了！
//     2. 改變成員變數宣告順序 -> 成員 offset 改變！
//     3. 在中間插入新的 virtual function -> vtable slot 位移改變！
void api_vs_abi_demo() {
    // Output:
    // === API vs ABI ===
    //   API compatibility: caller compiles without code changes
    //   ABI compatibility: existing caller binary runs with new .so without recompiling
    //   ABI break triggers: changing sizeof(Class), member offsets, or vtable layout
    //
    cout << "=== API vs ABI ===" << endl;
    cout << "  API compatibility: caller compiles without code changes" << endl;
    cout << "  ABI compatibility: existing caller binary runs with new .so without recompiling" << endl;
    cout << "  ABI break triggers: changing sizeof(Class), member offsets, or vtable layout" << endl << endl;
}

// ============================================================
// 2. Pimpl (Pointer to Implementation) Idiom
// ============================================================
// 本章重點：
//   在 public header 只放一個 forward declaration `struct Impl;` 與 `unique_ptr<Impl> impl_;`。
//   好處：
//     1. Public class 的 sizeof 永遠只有 1 個指標大小（64-bit 上固定 8 bytes）。
//     2. Impl 裡面怎麼加減欄位、換資料結構，都不影響 public class 的 binary layout。
//     3. Public header 不需要 #include 內部重型標頭檔，大幅加速編譯。

// --- 以下相當於放在公開 Header (camera_device.h) ---
class CameraDevice {
public:
    explicit CameraDevice(string sensor_name);
    ~CameraDevice();

    // Pimpl 搭配 unique_ptr 時，可支援 move
    CameraDevice(CameraDevice&&) noexcept;
    CameraDevice& operator=(CameraDevice&&) noexcept;

    void OpenSession(int fps);
    string StatusSummary() const;

private:
    struct Impl;               // Forward declaration：呼叫端不需要知道裡面長怎樣
    unique_ptr<Impl> impl_;    // 固定只有一個指標大小
};

// --- 以下相當於放在隱藏的 Source File (camera_device.cpp) ---
struct CameraDevice::Impl {
    string sensor_name;
    int current_fps = 0;
    bool active = false;
    vector<int> internal_lut{100, 200, 400};  // 隨便新增欄位都不影響 sizeof(CameraDevice)
};

CameraDevice::CameraDevice(string sensor_name)
    : impl_(make_unique<Impl>()) {
    impl_->sensor_name = std::move(sensor_name);
}

// 注意：~CameraDevice() 必須定義在看得見完整 struct Impl 的 .cpp 裡！
CameraDevice::~CameraDevice() = default;
CameraDevice::CameraDevice(CameraDevice&&) noexcept = default;
CameraDevice& CameraDevice::operator=(CameraDevice&&) noexcept = default;

void CameraDevice::OpenSession(int fps) {
    impl_->current_fps = fps;
    impl_->active = true;
}

string CameraDevice::StatusSummary() const {
    return impl_->sensor_name + " active=" + (impl_->active ? "true" : "false") +
           " fps=" + to_string(impl_->current_fps);
}

void pimpl_demo() {
    // Output:
    // === Pimpl Idiom ===
    //   sizeof(CameraDevice) = 8 (fixed pointer size)
    //   IMX989 active=true fps=60
    //
    cout << "=== Pimpl Idiom ===" << endl;
    cout << "  sizeof(CameraDevice) = " << sizeof(CameraDevice)
         << " (fixed pointer size)" << endl;

    CameraDevice dev("IMX989");
    dev.OpenSession(60);
    cout << "  " << dev.StatusSummary() << endl << endl;
}

// ============================================================
// 3. CMake vs Android Soong (Android.bp)
// ============================================================
// 本章重點：
//   一般跨平台專案用 CMake (CMakeLists.txt)；
//   Android OS (AOSP / Vendor HAL) 則使用 Soong (Android.bp)，概念完全可以一對一對照：
//
//   CMake:
//     add_library(sensor_core SHARED src/sensor.cpp)
//     target_include_directories(sensor_core PUBLIC include)
//     target_link_libraries(sensor_core PRIVATE liblog)
//
//   Android.bp (Soong):
//     cc_library_shared {
//         name: "libsensor_core",
//         vendor: true,
//         srcs: ["src/sensor.cpp"],
//         export_include_dirs: ["include"],
//         shared_libs: ["libbase", "liblog"],
//         cflags: ["-Wall", "-Wextra", "-Werror"],
//     }
void soong_vs_cmake_demo() {
    // Output:
    // === CMake vs Android.bp (Soong) ===
    //   add_library(SHARED)          <-> cc_library_shared
    //   add_executable()             <-> cc_binary
    //   add_test()                   <-> cc_test
    //   target_include_directories() <-> export_include_dirs / local_include_dirs
    //   target_link_libraries()      <-> shared_libs / static_libs / header_libs
    //
    cout << "=== CMake vs Android.bp (Soong) ===" << endl;
    cout << "  add_library(SHARED)          <-> cc_library_shared" << endl;
    cout << "  add_executable()             <-> cc_binary" << endl;
    cout << "  add_test()                   <-> cc_test" << endl;
    cout << "  target_include_directories() <-> export_include_dirs / local_include_dirs" << endl;
    cout << "  target_link_libraries()      <-> shared_libs / static_libs / header_libs" << endl << endl;
}

int main() {
    api_vs_abi_demo();
    pimpl_demo();
    soong_vs_cmake_demo();
    return 0;
}
