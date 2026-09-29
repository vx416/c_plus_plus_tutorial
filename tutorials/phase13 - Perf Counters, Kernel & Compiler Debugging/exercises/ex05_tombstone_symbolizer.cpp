/**
 * 練習 5: Tombstone PC Symbolizer 與 ARM64 HWASan Pointer Tag 檢查器
 *
 * 1. 實作 ResolveSymbol(symbols, pc)：找出 `start <= pc < start + size` 的符號名稱，
 *    找不到時回傳 "unknown"。
 * 2. 實作 IsHwasanAccessValid(tagged_ptr, memory_tag)：
 *    取出 tagged_ptr 最高 8 bits (Bits [63:56]) 與 memory_tag 比對是否一致。
 */

#include <cassert>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

struct SymbolRange {
    uint64_t start;
    uint64_t size;
    string name;
};

string ResolveSymbol(const vector<SymbolRange>& symbols, uint64_t pc) {
    for (const auto& s : symbols) {
        if (pc >= s.start && pc < s.start + s.size) {
            return s.name;
        }
    }
    return "unknown";
}

bool IsHwasanAccessValid(uintptr_t tagged_ptr, uint8_t memory_tag) {
    uint8_t ptr_tag = static_cast<uint8_t>(tagged_ptr >> 56);
    return ptr_tag == memory_tag;
}

int main() {
    vector<SymbolRange> symtab{
        {0x1000, 0x100, "SensorDriver::ReadI2c"},
        {0x1100, 0x250, "SensorDriver::ApplyExposure"},
        {0x1400, 0x080, "SensorDriver::StreamOff"},
    };

    assert(ResolveSymbol(symtab, 0x1000) == "SensorDriver::ReadI2c");
    assert(ResolveSymbol(symtab, 0x1200) == "SensorDriver::ApplyExposure");
    assert(ResolveSymbol(symtab, 0x1350) == "unknown");

    uintptr_t tagged = (static_cast<uintptr_t>(0xB4) << 56) | 0x0000007f12345000ULL;
    assert(IsHwasanAccessValid(tagged, 0xB4));
    assert(!IsHwasanAccessValid(tagged, 0x00));  // Tag mismatch -> Use-After-Free

    cout << "ex05 passed!" << endl;
    return 0;
}
