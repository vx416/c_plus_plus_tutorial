/**
 * Phase 13-5: Binary Symbol Inspection, Tombstone Crash Decoding & ARM64 HWASan Pointer Tagging
 *
 * 當系統服務或 Camera HAL 發生 Native Crash 時，Android `debuggerd` 會在 `/data/tombstones/`
 * 產生一份 Tombstone 報告（同時印在 `logcat -b crash`）。
 *
 * 身為系統/HAL 開發者，你需要看懂三件事：
 *   1. Signal 類型與 Fault Address 的意義（為什麼 `0x0000000000000018` 也是空指標 `nullptr`？）
 *   2. Symbol Table 與 Name Mangling：如何把 `pc 0x0001a420 liblyric.so` 還原成 C++ 函式與行號？
 *   3. ARM64 HWASan (Hardware-Assisted AddressSanitizer)：Pixel 手機如何利用指標最高 byte (TBI)
 *      在極低記憶體開銷下抓出 Use-After-Free 與 Buffer Overflow？
 */

#include <cstdint>
#include <iomanip>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>
using namespace std;

// ============================================================
// 1. Tombstone Signal 與 Struct Offset 空指標判讀
// ============================================================
// 本章重點：
//   看到 `signal 11 (SIGSEGV), code 1 (SEGV_MAPERR), fault addr 0x0000000000000018` 時，
//   新手常疑惑：「位址是 0x18，不是 0x0，為什麼說是 nullptr 解參考？」
//   原因：
//     假設 `SensorContext* ctx = nullptr;`，而你存取 `ctx->frame_count`：
//     如果 `frame_count` 在 `struct SensorContext` 裡的 offset 剛好是 24 bytes (0x18)，
//     CPU 就會直接去讀 `0x0 + 0x18 = 0x18`！
//     因此只要 `fault addr` 是很小的正整數（例如 `< 0x1000` 一頁以內），幾乎 100% 是 `nullptr->member`！
struct SensorContext {
    uint64_t magic;        // offset 0x00
    uint64_t handle;       // offset 0x08
    uint64_t timestamp;    // offset 0x10
    uint32_t frame_count;  // offset 0x18 (24 bytes)
};

void tombstone_signal_demo() {
    // Output:
    // === Tombstone Signal & Null-Offset Diagnosis ===
    //   offsetof(SensorContext, frame_count) = 0x18
    //   If ctx == nullptr, accessing ctx->frame_count faults at 0x0 + 0x18 = 0x18!
    //   Common Signals:
    //     SIGSEGV (SEGV_MAPERR) : unmapped address (nullptr + offset, or wild pointer)
    //     SIGSEGV (SEGV_ACCERR) : permission error (writing to read-only memory / code page)
    //     SIGABRT               : CHECK() failure, abort(), or allocator double-free
    //     SIGBUS  (BUS_ADRALN)  : unaligned memory access or truncated mmap file
    //
    cout << "=== Tombstone Signal & Null-Offset Diagnosis ===" << endl;
    cout << "  offsetof(SensorContext, frame_count) = 0x"
         << hex << offsetof(SensorContext, frame_count) << dec << endl;
    cout << "  If ctx == nullptr, accessing ctx->frame_count faults at 0x0 + 0x18 = 0x18!" << endl;
    cout << "  Common Signals:" << endl;
    cout << "    SIGSEGV (SEGV_MAPERR) : unmapped address (nullptr + offset, or wild pointer)" << endl;
    cout << "    SIGSEGV (SEGV_ACCERR) : permission error (writing to read-only memory / code page)" << endl;
    cout << "    SIGABRT               : CHECK() failure, abort(), or allocator double-free" << endl;
    cout << "    SIGBUS  (BUS_ADRALN)  : unaligned memory access or truncated mmap file" << endl << endl;
}

// ============================================================
// 2. Symbol Resolution (`addr2line` / `llvm-symbolizer` 原理)
// ============================================================
// 本章重點：
//   當 stripped binary 崩潰時，Tombstone 只印出相對 `.so` 起始位址的相對 PC (Relative PC)：
//     `#00 pc 000000000001a420 /vendor/lib64/libcamera_hal.so`
//   除錯工具（`llvm-symbolizer -e symbols/vendor/lib64/libcamera_hal.so 0x1a420`）做的事就是：
//     1. 在 unstripped ELF 的 Symbol Table (`.symtab` / DWARF `.debug_line`) 找哪個函式區間
//        `[start_addr, start_addr + size)` 包含 `0x1a420`。
//     2. 將 Itanium C++ ABI Mangled Name（如 `_ZN5lyric6Sensor8StreamOnEi`）Demangle 回可讀名稱。
struct ElfSymbolEntry {
    uint64_t start_addr;
    uint64_t size;
    string mangled_name;
    string demangled_name;
};

string symbolize_pc(const vector<ElfSymbolEntry>& symtab, uint64_t pc_offset) {
    for (const auto& sym : symtab) {
        if (pc_offset >= sym.start_addr && pc_offset < sym.start_addr + sym.size) {
            uint64_t delta = pc_offset - sym.start_addr;
            return sym.demangled_name + " + 0x" +
                   ([](uint64_t v) {
                       char buf[32];
                       snprintf(buf, sizeof(buf), "%llx", static_cast<unsigned long long>(v));
                       return string(buf);
                   })(delta) +
                   " (" + sym.mangled_name + ")";
        }
    }
    return "<unknown symbol>";
}

void symbol_resolution_demo() {
    // Output:
    // === Symbol Resolution & C++ Demangling ===
    //   pc 0x1a420 -> lyric::Sensor::StreamOn(int) + 0x20 (_ZN5lyric6Sensor8StreamOnEi)
    //   CLI tools:
    //     nm -C libfoo.so                (list symbols demangled)
    //     c++filt _ZN5lyric6Sensor8StreamOnEi
    //     llvm-symbolizer -Cfie unstripped/libfoo.so 0x1a420
    //
    cout << "=== Symbol Resolution & C++ Demangling ===" << endl;

    vector<ElfSymbolEntry> symtab{
        {0x1a000, 0x400, "_ZN5lyric6Sensor4InitEv", "lyric::Sensor::Init()"},
        {0x1a400, 0x180, "_ZN5lyric6Sensor8StreamOnEi", "lyric::Sensor::StreamOn(int)"},
    };

    cout << "  pc 0x1a420 -> " << symbolize_pc(symtab, 0x1a420) << endl;
    cout << "  CLI tools:" << endl;
    cout << "    nm -C libfoo.so                (list symbols demangled)" << endl;
    cout << "    c++filt _ZN5lyric6Sensor8StreamOnEi" << endl;
    cout << "    llvm-symbolizer -Cfie unstripped/libfoo.so 0x1a420" << endl << endl;
}

// ============================================================
// 3. ARM64 HWASan / MTE Pointer Tagging 原理
// ============================================================
// 本章重點：
//   64-bit ARMv8 CPU 的虛擬位址其實只用到低 48 bits，最高 8 bits (Bits [63:56]) 硬體預設忽略（Top-Byte Ignore, TBI）。
//   Android 的 HWASan (Hardware-Assisted ASan) 與 ARMv9 MTE (Memory Tagging Extension) 利用這個特性：
//     1. `malloc` 配置記憶體時，隨機產生一個 8-bit Tag（例如 `0xA5`），塞進回傳指標的最高 byte：
//        `0xA500007f8a401000`，並把同一個 Tag `0xA5` 記在對應的 Shadow Memory。
//     2. `free` 釋放記憶體時，把 Shadow Memory 的 Tag 改成另一個值（例如 `0x3C`）。
//     3. 如果程式發生 **Use-After-Free**，拿著舊指標（Tag `0xA5`）去讀寫已經變成 `0x3C` 的記憶體，
//        Pointer Tag (0xA5) != Memory Tag (0x3C)，當場抓到並印出精確的分配與釋放 stack！
uintptr_t pack_tagged_pointer(uintptr_t raw_addr, uint8_t tag) {
    constexpr uintptr_t kAddressMask = (1ULL << 56) - 1;
    return (raw_addr & kAddressMask) | (static_cast<uintptr_t>(tag) << 56);
}

uint8_t extract_pointer_tag(uintptr_t tagged_ptr) {
    return static_cast<uint8_t>(tagged_ptr >> 56);
}

uintptr_t untag_pointer(uintptr_t tagged_ptr) {
    constexpr uintptr_t kAddressMask = (1ULL << 56) - 1;
    return tagged_ptr & kAddressMask;
}

void hwasan_pointer_tagging_demo() {
    // Output:
    // === ARM64 HWASan Top-Byte Pointer Tagging ===
    //   raw addr   = 0x7f8a401000
    //   tagged ptr = 0xa500007f8a401000 (tag = 0xa5)
    //   after free -> memory tag changed to 0x3c -> tag mismatch (0xa5 != 0x3c) catches Use-After-Free!
    //
    cout << "=== ARM64 HWASan Top-Byte Pointer Tagging ===" << endl;

    uintptr_t raw_addr = 0x0000007f8a401000ULL;
    uint8_t alloc_tag = 0xA5;
    uintptr_t tagged_ptr = pack_tagged_pointer(raw_addr, alloc_tag);

    cout << "  raw addr   = 0x" << hex << untag_pointer(tagged_ptr) << endl;
    cout << "  tagged ptr = 0x" << tagged_ptr
         << " (tag = 0x" << static_cast<int>(extract_pointer_tag(tagged_ptr)) << ")" << dec << endl;

    uint8_t shadow_memory_tag_after_free = 0x3C;
    if (extract_pointer_tag(tagged_ptr) != shadow_memory_tag_after_free) {
        cout << "  after free -> memory tag changed to 0x3c -> tag mismatch (0xa5 != 0x3c) catches Use-After-Free!" << endl;
    }
    cout << endl;
}

int main() {
    tombstone_signal_demo();
    symbol_resolution_demo();
    hwasan_pointer_tagging_demo();
    return 0;
}
