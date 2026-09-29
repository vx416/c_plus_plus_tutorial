# Phase 13 - Perf Counters, Kernel & Compiler Debugging

## 定位

這個 Phase 聚焦在 **Android OS / Linux Kernel Driver / C++ 編譯器與硬體效能除錯（System-Level Observability, Profiling & Crash Triage）**。
當程式碼邏輯看起來沒問題，但在實機上遇到 **掉幀 (Frame Drop)、CPU 飆高、I2C/Driver Hang 死、`-O2` 優化怪病、或 Tombstone Crash** 時，這裡涵蓋了底層原理與工具鏈實戰。

## 目錄結構

```
phase13 - Perf Counters, Kernel & Compiler Debugging/
├── 01_perfetto_tracing_and_counters.cpp     # Perfetto / ftrace trace_marker: B/E, Async S/F, Counter C
├── 02_pmu_perf_counters_and_cache.cpp       # steady_clock, P99 latency, PMU IPC / Cache Miss / False Sharing
├── 03_kernel_driver_debugging.cpp           # MMIO volatile, D-state hang (-ETIMEDOUT), Flight Recorder, debugfs
├── 04_compiler_optimization_and_ub_traps.cpp # -O2 UB traps, Strict Aliasing (std::bit_cast), DoNotOptimize
├── 05_binary_symbols_tombstone_and_hwasan.cpp # Tombstone SIGSEGV offset, Symbolizer, ARM64 HWASan Pointer Tagging
├── exercises/
│   ├── ex01_perfetto_trace_builder.cpp
│   ├── ex02_false_sharing_and_cache_line.cpp
│   ├── ex03_mmio_and_flight_recorder.cpp
│   ├── ex04_strict_aliasing_and_bit_cast.cpp
│   └── ex05_tombstone_symbolizer.cpp
├── Makefile
└── README.md
```

## 使用方式

```bash
cd "tutorials/phase13 - Perf Counters, Kernel & Compiler Debugging"

make
make run TARGET=01_perfetto_tracing_and_counters
make run-examples
make run-exercises
make sanitize TARGET=04_compiler_optimization_and_ub_traps
make clean
```

## 核心工具鏈與觀念速查

### 1. Perfetto & Ftrace (`trace_marker`) 四大事件格式

| 類型 | 格式 | 對應 Android 巨集 | 適用場景 |
|---|---|---|---|
| **Sync Slice** | `B\|<pid>\|<name>` / `E\|<pid>` | `ATRACE_CALL()` / `ATRACE_BEGIN()` | **同一個執行緒**內的函式耗時區間（自動堆疊嵌套） |
| **Async Slice** | `S\|<pid>\|<name>\|<cookie>` / `F\|...` | `ATRACE_ASYNC_BEGIN(name, cookie)` | **跨執行緒**生命週期（如 `cookie = frame_number` 追蹤整張 Frame 從 Request 到 Callback） |
| **Counter Track** | `C\|<pid>\|<name>\|<value>` | `ATRACE_INT(name, val)` | 在 Perfetto 畫數值折線/階梯圖（如 `InFlightRequests`、`FreeBuffers`、`ExposureUs`） |
| **Instant Event** | `I\|<pid>\|<name>` | `ATRACE_INSTANT(name)` | 標記單一時間點事件（如 VSync / SOF 中斷觸發） |

### 2. CPU PMU Perf Counters (`simpleperf` / `perf`) 判讀指南

```bash
# Android 實機採樣 PMU 硬體計數器
adb shell simpleperf stat -p <pid> -e cpu-cycles,instructions,cache-misses,branch-misses --duration 5

# 錄製 Call Graph 火焰圖（編譯時需帶 -fno-omit-frame-pointer 或使用 --call-graph dwarf）
adb shell simpleperf record -p <pid> -g --duration 5 -o /data/local/tmp/perf.data
```

* **`IPC = instructions / cpu-cycles`**：
  * **IPC 高 (> 2.0)**：CPU 算力跑滿（Compute-bound），需減少指令數或改用 ARM NEON SIMD。
  * **IPC 低 (< 0.5) 且 `cache-misses` 高**：CPU 都在空轉等 DRAM（Memory-bound），檢查資料結構 Spatial Locality 或多執行緒 **False Sharing (`alignas(64)`)**。
* **`cpu-cycles` 低但 Wall-clock 時間很長**：執行緒處於 Off-CPU（等 Mutex Lock、等 Kernel `ioctl` / I2C，或被排程到小核 / 降頻）。

### 3. Kernel Driver、Compiler 與 Crash Triage 關鍵命令

```bash
# 1. 將 Tombstone 的相對 PC 位址 (如 0x1a420) 還原為 C++ 檔名與行號
llvm-symbolizer -Cfie out/target/product/<device>/symbols/vendor/lib64/libfoo.so 0x1a420

# 2. 解碼 C++ Mangled Symbol
c++filt _ZN5lyric6Sensor8StreamOnEi

# 3. 觀察編譯器 -O2 產生的組合語言與符號表
objdump -d -C -S build/04_compiler_optimization_and_ub_traps
nm -C -n build/05_binary_symbols_tombstone_and_hwasan

# 4. 抓取卡死行程 (Watchdog / Deadlock) 所有執行緒的即時 Native Call Stack
adb shell debuggerd -b <pid>
```

## 練習題說明

| 練習 | 對應章節 | 題目 | 難度 |
|------|---------|------|------|
| ex01 | 01 Perfetto | 實作 `trace_marker` 產生器並偵測未關閉的 Async Slice (Frame Leak) | 中等 |
| ex02 | 02 PMU & Cache | 用 `alignas(64)` 消除 False Sharing 並計算 IPC 與 Cache Miss Rate | 簡單 |
| ex03 | 03 Kernel/Driver | 實作 `volatile` MMIO Timeout Polling 與 2 次方容量 Ring Buffer Flight Recorder | 中等 |
| ex04 | 04 Compiler Traps | 用 C++20 `std::bit_cast` 安全擷取 IEEE-754 Float 指數欄位（無 Strict Aliasing UB） | 簡單 |
| ex05 | 05 Tombstone/HWASan | 實作 ELF Symbol 區間查找器與 ARM64 HWASan Pointer Tag 驗證器 | 中等 |
