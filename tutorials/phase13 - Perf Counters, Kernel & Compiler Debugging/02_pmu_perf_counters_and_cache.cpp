/**
 * Phase 13-2: Hardware PMU Perf Counters (`simpleperf` / `perf`), IPC, Cache Miss & False Sharing
 *
 * 當你在 Perfetto 看到某個 C++ 函式從 2ms 暴增到 12ms 時，要怎麼知道「為什麼變慢」？
 * 這時候就要靠 CPU 內建的 PMU (Performance Monitoring Unit) 硬體計數器，
 * 在 Linux 用 `perf stat`、在 Android 用 `simpleperf stat`（底層系統呼叫為 `perf_event_open`）。
 *
 * 目錄:
 *   1. Monotonic Clock (`steady_clock` vs `system_clock`) 與 P50/P99 Tail Latency
 *   2. 四大核心硬體 Perf Counters：Cycles, Instructions, IPC, Cache Misses, Branch Misses
 *   3. 為什麼「同樣的演算法」耗時差 5 倍？—— Spatial Locality 與 Cache Miss
 *   4. 多執行緒隱形殺手：False Sharing（偽共享）與 `alignas(64)` 解法
 */

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <thread>
#include <vector>
using namespace std;

// ============================================================
// 1. Monotonic Clock & Tail Latency (P50 / P90 / P99)
// ============================================================
// 本章重點：
//   - 絕對不要用 std::chrono::system_clock 測量耗時！因為 NTP 網路校時會讓時間倒退或跳躍。
//   - 一律用 std::chrono::steady_clock（對應 Linux CLOCK_MONOTONIC）。
//   - 在即時系統（如 60fps 相機每幀預算 16.6ms）中，「平均耗時 (Average)」會騙人：
//     99 幀花 1ms、1 幀花 50ms，平均才 1.49ms，但使用者已經看到明顯掉幀！
//     因此實務上一律看 P90 / P99 / Max (Tail Latency)。
struct LatencySummary {
    int64_t p50_us;
    int64_t p90_us;
    int64_t p99_us;
    int64_t max_us;
};

LatencySummary compute_percentiles(vector<int64_t> samples_us) {
    if (samples_us.empty()) return {0, 0, 0, 0};
    sort(samples_us.begin(), samples_us.end());
    const size_t n = samples_us.size();
    return LatencySummary{
        samples_us[(n * 50) / 100],
        samples_us[(n * 90) / 100],
        samples_us[(n * 99) / 100],
        samples_us.back(),
    };
}

void tail_latency_demo() {
    // Output:
    // === Monotonic Clock & Tail Latency (P50 / P90 / P99) ===
    //   P50 = 1000 us, P90 = 1000 us, P99 = 35000 us, Max = 35000 us
    //   (Average looks fine, but P99 reveals the 35ms frame drop spike!)
    //
    cout << "=== Monotonic Clock & Tail Latency (P50 / P90 / P99) ===" << endl;

    vector<int64_t> frame_times_us(100, 1000);  // 99 幀都是 1000 us (1 ms)
    frame_times_us[99] = 35000;                 // 第 100 幀因為 GC / Lock / Thermal 暴衝到 35 ms

    LatencySummary stats = compute_percentiles(frame_times_us);
    cout << "  P50 = " << stats.p50_us << " us, P90 = " << stats.p90_us
         << " us, P99 = " << stats.p99_us << " us, Max = " << stats.max_us << " us" << endl;
    cout << "  (Average looks fine, but P99 reveals the 35ms frame drop spike!)" << endl << endl;
}

// ============================================================
// 2. 硬體 PMU Perf Counters 判讀模型 (`simpleperf stat`)
// ============================================================
// 本章重點：
//   當你對 Android 行程執行：
//     adb shell simpleperf stat -p <pid> -e cpu-cycles,instructions,cache-misses,branch-misses --duration 5
//
//   怎麼看數字？
//     1. IPC (Instructions Per Cycle = instructions / cpu-cycles)：
//        - IPC 高 (例如 > 2.0)：CPU 管線跑得很滿，屬於 Compute-bound（若還嫌慢，要減少指令數或用 NEON SIMD）。
//        - IPC 極低 (例如 < 0.4)：CPU 大部分 cycle 都在 Stall（空轉等資料），通常是 **Cache Miss (Memory-bound)**！
//     2. cpu-cycles 沒變多，但 wall-clock time 變長很多：
//        - 代表不是指令變多，而是：
//          (a) CPU 被降頻（Thermal throttling，或被排程到小核 Little Core）
//          (b) 執行緒處於 Off-CPU 睡眠（例如等 Mutex lock 或等 Kernel ioctl / I2C）
struct PmuSnapshot {
    string scenario;
    uint64_t instructions;
    uint64_t cpu_cycles;
    uint64_t cache_misses;

    double ipc() const {
        return cpu_cycles > 0 ? static_cast<double>(instructions) / cpu_cycles : 0.0;
    }
};

void pmu_interpretation_demo() {
    // Output:
    // === PMU Perf Counters Interpretation (simpleperf stat) ===
    //   Row-major scan (Cache-friendly): IPC = 2.50, cache_misses = 1200
    //   Pointer chasing (Cache-miss):    IPC = 0.25, cache_misses = 480000 (CPU stalled on DRAM!)
    //
    cout << "=== PMU Perf Counters Interpretation (simpleperf stat) ===" << endl;

    PmuSnapshot row_major{"Row-major scan (Cache-friendly)", 10'000'000, 4'000'000, 1'200};
    PmuSnapshot ptr_chase{"Pointer chasing (Cache-miss)   ", 10'000'000, 40'000'000, 480'000};

    cout << fixed << setprecision(2);
    cout << "  " << row_major.scenario << ": IPC = " << row_major.ipc()
         << ", cache_misses = " << row_major.cache_misses << endl;
    cout << "  " << ptr_chase.scenario << ": IPC = " << ptr_chase.ipc()
         << ", cache_misses = " << ptr_chase.cache_misses
         << " (CPU stalled on DRAM!)" << endl << endl;
}

// ============================================================
// 3. 多執行緒 False Sharing（偽共享）與 Cache Line Alignment
// ============================================================
// 本章重點：
//   現代 ARM64 / x86 CPU 的 L1 Cache 是以 **Cache Line (通常 64 bytes)** 為單位維護一致性 (MESI protocol)。
//   如果 Thread 0 狂寫 `counters[0]`、Thread 1 狂寫 `counters[1]`，
//   雖然它們是兩個獨立的 `atomic<uint64_t>`，但因為緊鄰在一起（共 16 bytes），落在**同一個 64-byte Cache Line**！
//   結果兩顆 CPU 核心的 L1 Cache Line 不斷互相 invalidate（Cache Ping-Pong），效能暴跌！
//
//   解法：用 `alignas(64)` 讓每個跨執行緒高頻寫入的計數器獨佔一條 Cache Line。
struct FalseSharedCounters {
    atomic<uint64_t> worker0_count{0};
    atomic<uint64_t> worker1_count{0};  // 與 worker0_count 擠在同一條 64-byte cache line！
};

struct CacheAlignedCounters {
    alignas(64) atomic<uint64_t> worker0_count{0};
    alignas(64) atomic<uint64_t> worker1_count{0};  // 各自獨立一條 64-byte cache line
};

void false_sharing_demo() {
    // Output:
    // === False Sharing vs Cache-Line Aligned (alignas(64)) ===
    //   sizeof(FalseSharedCounters)  = 16 (shares single 64B cache line -> cache ping-pong)
    //   sizeof(CacheAlignedCounters) = 128 (isolated on separate 64B cache lines)
    //
    cout << "=== False Sharing vs Cache-Line Aligned (alignas(64)) ===" << endl;
    cout << "  sizeof(FalseSharedCounters)  = " << sizeof(FalseSharedCounters)
         << " (shares single 64B cache line -> cache ping-pong)" << endl;
    cout << "  sizeof(CacheAlignedCounters) = " << sizeof(CacheAlignedCounters)
         << " (isolated on separate 64B cache lines)" << endl << endl;
}

int main() {
    tail_latency_demo();
    pmu_interpretation_demo();
    false_sharing_demo();
    return 0;
}
