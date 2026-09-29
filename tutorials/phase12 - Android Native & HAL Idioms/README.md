# Phase 12 - Android Native & HAL Idioms

## 定位

這個 Phase 針對 **Android OS Native 層（AOSP `libbase` / `libutils` / `libbinder`）與硬體抽象層（如 Pixel Camera HAL `LyricHal`、AIDL HAL）** 的工程實務所設計。

注意：本目錄的 `Makefile` 直接開啟了與 Android 系統相同的編譯限制：

```bash
CXXFLAGS = -std=c++20 -Wall -Wextra -Wthread-safety -fno-exceptions -fno-rtti -pthread
```

所有範例與練習題皆在 **禁止 `try/catch` (`-fno-exceptions`)** 與 **禁止 `dynamic_cast`/`typeid` (`-fno-rtti`)** 的真實條件下編譯執行。

## 目錄結構

```
phase12 - Android Native & HAL Idioms/
├── 01_fno_exceptions_and_sigil_factory.cpp # -fno-exceptions, StatusOr<T>, Sigil Passkey Factory
├── 02_intrusive_refbase_sp_wp.cpp          # android::RefBase / sp<T> / onFirstRef / Custom Deleter
├── 03_binder_aidl_ipc_shape.cpp            # AIDL BpProxy & BnStub / SCM_RIGHTS 跨行程傳遞 FD
├── 04_hal_session_and_scoped_trace.cpp     # HAL3 Async Callback / ThreadPool::EnqueueWork / ATRACE_CALL
├── exercises/
│   ├── ex01_sigil_status_or_factory.cpp
│   ├── ex02_intrusive_sp.cpp
│   ├── ex03_fd_passing_scm_rights.cpp
│   └── ex04_scoped_atrace_and_callback.cpp
├── Makefile
└── README.md
```

## 使用方式

```bash
cd "tutorials/phase12 - Android Native & HAL Idioms"

make
make run TARGET=01_fno_exceptions_and_sigil_factory
make run-examples
make run-exercises
make sanitize TARGET=ex02_intrusive_sp
make clean
```

## 核心觀念速查

| Android / HAL 慣用法 | 解決什麼問題？ | 對應 AOSP / Google 元件 |
|---|---|---|
| **`Status` / `StatusOr<T>` + `RETURN_IF_ERROR`** | 在 `-fno-exceptions` 下型別安全地傳遞錯誤原因，並用 `[[nodiscard]]` 防止忽略錯誤 | `absl::StatusOr<T>`、`android::base::Result<T>`、`ndk::ScopedAStatus` |
| **Static Factory `Create()` + `Sigil` Passkey** | Constructor 不能 throw，又不想用容易忘記呼叫的兩階段 `Init()`；用私有 `Sigil` 鎖住公開 ctor 以搭配 `std::make_unique` | `LyricHal` / Abseil Passkey Idiom |
| **Intrusive `RefBase` + `sp<T>` / `wp<T>`** | 引用計數嵌在物件內，`sizeof(sp<T>) == 8`，支援從 `this` 轉回 `sp<T>` 及 `onFirstRef()` 初始化鉤子 | `utils/RefBase.h` (`android::sp<T>`)、NDK `ndk::SharedRefBase` |
| **AIDL `Bp*` (Proxy) / `Bn*` (Stub) + FD 傳遞** | 跨行程 RPC 介面分層，並透過 Kernel (`Binder` / `SCM_RIGHTS`) 把 `DMA-BUF` / `AHardwareBuffer` 的 FD 傳給另一行程做 `mmap` 零拷貝 | `libbinder_ndk`、`ndk::ScopedFileDescriptor` |
| **`ThreadPool::EnqueueWork` + `ScopedTrace`** | 取代不受控的 `std::async` 處理非同步 HAL Request/Result，並以 RAII 巨集自動打上成對的 Perfetto `B\|` / `E\|` trace 標記 | `ATRACE_CALL()` (`cutils/trace.h`)、HAL Session ThreadPool |

## 練習題說明

| 練習 | 對應章節 | 題目 | 難度 |
|------|---------|------|------|
| ex01 | 01 Sigil & StatusOr | 用 `Sigil` + `StatusOr<unique_ptr<T>>` 建立 `SensorModeTable` | 中等 |
| ex02 | 02 Intrusive `sp<T>` | 實作 `LightRefBase` (`onFirstRef`) 與 `sp<T>` 生命週期管理 | 中等 |
| ex03 | 03 IPC FD Passing | 用 `socketpair` + `SCM_RIGHTS` 跨行程傳遞 FD 並以 `mmap` 零拷貝驗證 | 困難 |
| ex04 | 04 ScopedTrace | 實作 RAII `ScopedTrace` (`ATRACE_CALL`) 確保提早 return 仍成對閉合 | 簡單 |
