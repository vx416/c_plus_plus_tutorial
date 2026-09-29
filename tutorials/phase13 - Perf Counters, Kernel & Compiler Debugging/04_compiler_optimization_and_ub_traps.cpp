/**
 * Phase 13-4: Compiler Optimization Traps (`-O0` vs `-O2`), Strict Aliasing & Benchmark Sinks
 *
 * 工程師最頭痛的 bug 之一是：
 *   「Debug build (`-O0`) 跑起來完全正常，一換成 Release build (`-O2` / `-O3`) 就結果錯誤或 Crash！」
 *
 * 99% 的情況不是編譯器有 bug，而是程式碼踩到了 **Undefined Behavior (UB)**，
 * 編譯器在 `-O2` 下有權「假設 UB 永遠不會發生」並據此把你的分支或記憶體讀寫直接刪掉！
 *
 * 目錄:
 *   1. Strict Aliasing Rule（嚴格別名規則）與 `std::bit_cast` / `memcpy` 正解
 *   2. Signed Integer Overflow UB 如何導致安全檢查被 `-O2` 整個拔除
 *   3. Dead-Store Elimination（死寫入消除）：為什麼 `memset` 清空密碼或 micro-benchmark 會被編譯器刪光？
 *   4. Compiler Hints 與可除錯性：`[[likely]]`, `__attribute__((noinline))`, `-fno-omit-frame-pointer`
 */

#include <bit>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <limits>
using namespace std;

// ============================================================
// 1. Strict Aliasing Rule 與 std::bit_cast (C++20)
// ============================================================
// 本章重點：
//   在 C/C++ 中，拿 `uint32_t*` 去指一個 `float` 變數並解參考：
//     float f = 1.0f;
//     uint32_t bits = *reinterpret_cast<uint32_t*>(&f); // UB! 違反 Strict Aliasing!
//   編譯器在 -O2 下開啟 `-fstrict-aliasing`，認為 `uint32_t*` 與 `float*` 絕不可能指向同一個位址，
//   因此可能重排讀寫順序，讀出未初始化的垃圾值！
//
//   合法的 Type Punning（型別雙關）只有兩種：
//     1. C++20 `std::bit_cast<To>(from)`（編譯期檢查 sizeof 相等，直接編成單一暫存器搬移指令）
//     2. `std::memcpy(&dst, &src, sizeof(dst))`（編譯器認得 memcpy，在 -O2 下完全不會產生函式呼叫，直接變成 mov 指令！）
void strict_aliasing_demo() {
    // Output:
    // === Strict Aliasing & std::bit_cast ===
    //   float 1.0f IEEE-754 hex = 0x3f800000
    //   reconstructed float     = 1
    //
    cout << "=== Strict Aliasing & std::bit_cast ===" << endl;

    float original = 1.0f;
    uint32_t raw_bits = std::bit_cast<uint32_t>(original);
    float restored = std::bit_cast<float>(raw_bits);

    cout << "  float 1.0f IEEE-754 hex = 0x" << hex << raw_bits << dec << endl;
    cout << "  reconstructed float     = " << restored << endl << endl;
}

// ============================================================
// 2. Signed Overflow UB 導致邊界檢查被 -O2 刪除
// ============================================================
// 本章重點：
//   無號數 (uint32_t) 溢位是明確定義的模數環繞 (Modulo wrapping)；
//   但有號數 (int32_t) 溢位是 Undefined Behavior！
//
//   危險寫法：
//     bool bad_check(int32_t offset, int32_t len) {
//         return (offset + len) < offset; // 編譯器認為有號數 x + y 永遠不可能溢位，直接把這行優化成 return false!
//     }
//   正確寫法：先檢查 `len > MAX - offset`，或轉成無號數 / 更大寬度整數再檢查。
bool safe_add_i32(int32_t a, int32_t b, int32_t* out) {
    if (b > 0 && a > numeric_limits<int32_t>::max() - b) {
        return false;  // 防止發生 signed overflow UB
    }
    if (b < 0 && a < numeric_limits<int32_t>::min() - b) {
        return false;
    }
    *out = a + b;
    return true;
}

void signed_overflow_trap_demo() {
    // Output:
    // === Signed Overflow Trap in -O2 ===
    //   safe_add_i32(2000000000, 1000000000) ok = false (overflow prevented before UB!)
    //   safe_add_i32(100, 200)               ok = true, sum = 300
    //
    cout << "=== Signed Overflow Trap in -O2 ===" << endl;

    int32_t result = 0;
    bool ok1 = safe_add_i32(2'000'000'000, 1'000'000'000, &result);
    cout << "  safe_add_i32(2000000000, 1000000000) ok = "
         << boolalpha << ok1 << " (overflow prevented before UB!)" << endl;

    bool ok2 = safe_add_i32(100, 200, &result);
    cout << "  safe_add_i32(100, 200)               ok = "
         << ok2 << noboolalpha << ", sum = " << result << endl << endl;
}

// ============================================================
// 3. Dead-Store Elimination 與 DoNotOptimize
// ============================================================
// 本章重點：
//   當你寫一個 micro-benchmark 測試某個演算法的速度：
//     for (int i = 0; i < 1000000; ++i) { int r = compute(i); }
//   在 -O2 下，編譯器發現 `r` 算出來根本沒被使用，會把整個迴圈砍成 0 條指令（耗時 0 ns）！
//   在 Google Benchmark / 系統測試中，會用 inline assembly `asm volatile("" : : "r,m"(val) : "memory")`
//   告訴編譯器「這個值有外部觀察者會用，不准把計算過程優化掉」。
template <typename T>
inline void DoNotOptimize(const T& value) {
#if defined(__GNUC__) || defined(__clang__)
    asm volatile("" : : "r,m"(value) : "memory");
#else
    (void)value;
#endif
}

// __attribute__((noinline)) 可防止編譯器把想在 perf / stack trace 裡單獨看到的函式 inline 掉
#if defined(__GNUC__) || defined(__clang__)
#define NOINLINE __attribute__((noinline))
#else
#define NOINLINE
#endif

NOINLINE uint64_t benchmarked_kernel(uint64_t x) {
    return (x * 1664525ULL + 1013904223ULL);
}

void dead_store_and_attributes_demo() {
    // Output:
    // === Dead-Store Elimination & Compiler Attributes ===
    //   benchmarked_kernel(42) = 1083814273
    //   Key compiler flags for debugging/profiling:
    //     -fno-omit-frame-pointer : keeps FP register so simpleperf/perf can unwind call stacks fast
    //     -gline-tables-only      : adds file:line debug info with minimal binary size overhead
    //
    cout << "=== Dead-Store Elimination & Compiler Attributes ===" << endl;

    uint64_t val = benchmarked_kernel(42);
    DoNotOptimize(val);
    cout << "  benchmarked_kernel(42) = " << val << endl;
    cout << "  Key compiler flags for debugging/profiling:" << endl;
    cout << "    -fno-omit-frame-pointer : keeps FP register so simpleperf/perf can unwind call stacks fast" << endl;
    cout << "    -gline-tables-only      : adds file:line debug info with minimal binary size overhead" << endl;
    cout << endl;
}

int main() {
    strict_aliasing_demo();
    signed_overflow_trap_demo();
    dead_store_and_attributes_demo();
    return 0;
}
