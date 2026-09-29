/**
 * 練習 2: PMU Perf Counter 指標計算與 Cache-Line 對齊 (`alignas(64)`)
 *
 * 1. 讓 PaddedWorkerStats 的每個槽位對齊到 64-byte Cache Line 防止 False Sharing。
 * 2. 實作 ComputeIpc(instructions, cycles) 與 ComputeCacheMissRate(misses, references)。
 */

#include <atomic>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <iostream>
using namespace std;

struct alignas(64) PaddedWorkerSlot {
    atomic<uint64_t> processed_frames{0};
    atomic<uint64_t> dropped_frames{0};
};

struct PaddedWorkerTable {
    PaddedWorkerSlot workers[4];
};

double ComputeIpc(uint64_t instructions, uint64_t cycles) {
    if (cycles == 0) return 0.0;
    return static_cast<double>(instructions) / static_cast<double>(cycles);
}

double ComputeCacheMissRate(uint64_t cache_misses, uint64_t cache_references) {
    if (cache_references == 0) return 0.0;
    return static_cast<double>(cache_misses) / static_cast<double>(cache_references);
}

int main() {
    static_assert(alignof(PaddedWorkerSlot) == 64, "Each slot must be 64-byte aligned");
    static_assert(sizeof(PaddedWorkerSlot) == 64, "Each slot should occupy one full 64B cache line");
    static_assert(offsetof(PaddedWorkerTable, workers[1]) - offsetof(PaddedWorkerTable, workers[0]) == 64,
                  "Adjacent worker slots must be separated by 64 bytes");

    assert(abs(ComputeIpc(2000, 1000) - 2.0) < 1e-9);
    assert(ComputeIpc(100, 0) == 0.0);

    assert(abs(ComputeCacheMissRate(50, 1000) - 0.05) < 1e-9);
    assert(ComputeCacheMissRate(0, 0) == 0.0);

    cout << "ex02 passed!" << endl;
    return 0;
}
