/**
 * Phase 1-7: Memory Layout, Alignment & Bit Manipulation
 *
 * 在系統程式、Android HAL 與硬體驅動（如 Image Sensor I2C register、V4L2/LWIS ioctl payload、
 * EEPROM/OTP 解析）中，不能只知道「變數有型別」，還必須精確掌握：
 *   1. Fixed-width Integers（固定寬度整數 uint8_t / uint16_t / uint32_t / uint64_t）
 *   2. Struct Alignment & Padding（記憶體對齊與填充、sizeof、offsetof）
 *   3. alignas 與 __attribute__((packed))（控制對齊與緊湊結構）
 *   4. Bit Manipulation（位元遮罩、位移、Register R/W 慣用法）
 *   5. Endianness（位元組序 Little-Endian vs Big-Endian）
 */

#include <cstddef>
#include <cstdint>
#include <iomanip>
#include <iostream>
using namespace std;

// ============================================================
// 1. Fixed-width Integers
// ============================================================
// 本章重點：
//   C++ 的 int / long 在不同平台（32-bit vs 64-bit）大小可能不同。
//   只要碰到硬體暫存器、網路封包、IPC payload、檔案格式，一律用 <cstdint>：
//     uint8_t, uint16_t, uint32_t, uint64_t
//     int8_t,  int16_t,  int32_t,  int64_t
void fixed_width_demo() {
    // Output:
    // === Fixed-width Integers ===
    //   sizeof(uint8_t)  = 1
    //   sizeof(uint16_t) = 2
    //   sizeof(uint32_t) = 4
    //   sizeof(uint64_t) = 8
    //
    cout << "=== Fixed-width Integers ===" << endl;
    cout << "  sizeof(uint8_t)  = " << sizeof(uint8_t) << endl;
    cout << "  sizeof(uint16_t) = " << sizeof(uint16_t) << endl;
    cout << "  sizeof(uint32_t) = " << sizeof(uint32_t) << endl;
    cout << "  sizeof(uint64_t) = " << sizeof(uint64_t) << endl;
    cout << endl;
}

// ============================================================
// 2. Struct Alignment & Padding
// ============================================================
// 本章重點：
//   CPU 讀取對齊（aligned）的記憶體最有效率：
//     uint32_t (4 bytes) 通常對齊在 4 的倍數位址
//     uint64_t (8 bytes) 通常對齊在 8 的倍數位址
//
//   因此編譯器會在成員之間自動塞 padding bytes。
//   調整成員宣告順序（由大到小）可以在不損失效能下縮小 struct 大小。
struct PoorLayout {
    uint8_t a;   // 1 byte + 3 bytes padding
    uint32_t b;  // 4 bytes
    uint8_t c;   // 1 byte + 7 bytes padding
    uint64_t d;  // 8 bytes
};               // Total: 24 bytes

struct GoodLayout {
    uint64_t d;  // 8 bytes
    uint32_t b;  // 4 bytes
    uint8_t a;   // 1 byte
    uint8_t c;   // 1 byte + 2 bytes tail padding
};               // Total: 16 bytes

void alignment_padding_demo() {
    // Output:
    // === Struct Alignment & Padding ===
    //   sizeof(PoorLayout) = 24, alignof = 8
    //   PoorLayout offsets: a=0, b=4, c=8, d=16
    //   sizeof(GoodLayout) = 16, alignof = 8
    //
    cout << "=== Struct Alignment & Padding ===" << endl;
    cout << "  sizeof(PoorLayout) = " << sizeof(PoorLayout)
         << ", alignof = " << alignof(PoorLayout) << endl;
    cout << "  PoorLayout offsets: a=" << offsetof(PoorLayout, a)
         << ", b=" << offsetof(PoorLayout, b)
         << ", c=" << offsetof(PoorLayout, c)
         << ", d=" << offsetof(PoorLayout, d) << endl;
    cout << "  sizeof(GoodLayout) = " << sizeof(GoodLayout)
         << ", alignof = " << alignof(GoodLayout) << endl;
    cout << endl;
}

// ============================================================
// 3. Packed Struct vs alignas
// ============================================================
// 本章重點：
//   1. 當解析硬體 EEPROM / OTP binary blob 時，資料沒有 padding，
//      可用 __attribute__((packed)) 取消自動填充（注意：未對齊存取在某些架構較慢或有限制）。
//   2. 當傳遞給 DMA / SIMD / Cache line 時，可用 alignas(N) 強制對齊到更大邊界。
struct __attribute__((packed)) SensorOtpHeader {
    uint8_t version;
    uint16_t module_id;
    uint32_t calibration_crc;
};

struct alignas(64) CacheLineAlignedBuffer {
    uint32_t data[4];
};

void packed_and_alignas_demo() {
    // Output:
    // === Packed Struct vs alignas ===
    //   sizeof(SensorOtpHeader) = 7 (1 + 2 + 4, no padding)
    //   alignof(CacheLineAlignedBuffer) = 64, sizeof = 64
    //
    cout << "=== Packed Struct vs alignas ===" << endl;
    cout << "  sizeof(SensorOtpHeader) = " << sizeof(SensorOtpHeader)
         << " (1 + 2 + 4, no padding)" << endl;
    cout << "  alignof(CacheLineAlignedBuffer) = " << alignof(CacheLineAlignedBuffer)
         << ", sizeof = " << sizeof(CacheLineAlignedBuffer) << endl;
    cout << endl;
}

// ============================================================
// 4. Bit Manipulation & Register Fields
// ============================================================
// 本章重點：
//   硬體暫存器常把多個設定塞進同一個 16-bit 或 32-bit 整數：
//     Bit [0]     : STREAM_ON (1 bit)
//     Bit [1]     : HDR_ENABLE (1 bit)
//     Bit [7:4]   : GAIN_SHIFT (4 bits)
//     Bit [15:8]  : EXPOSURE_LINES_LOW (8 bits)
//
//   務必使用 unsigned 型別（如 uint32_t）搭配 1u << n，避免 signed overflow UB。
constexpr uint32_t kStreamOnBit = (1u << 0);
constexpr uint32_t kHdrEnableBit = (1u << 1);
constexpr uint32_t kGainShiftPos = 4;
constexpr uint32_t kGainShiftMask = (0xFu << kGainShiftPos);

uint32_t set_field(uint32_t reg, uint32_t mask, uint32_t pos, uint32_t val) {
    // 先清除舊欄位 (reg & ~mask)，再寫入新值 ((val << pos) & mask)
    return (reg & ~mask) | ((val << pos) & mask);
}

uint32_t get_field(uint32_t reg, uint32_t mask, uint32_t pos) {
    return (reg & mask) >> pos;
}

void bit_manipulation_demo() {
    // Output:
    // === Bit Manipulation (Register R/W) ===
    //   initial reg       = 0x0000
    //   enable stream+hdr = 0x0003
    //   set gain_shift=5  = 0x0053
    //   read gain_shift   = 5
    //   disable hdr       = 0x0051
    //
    cout << "=== Bit Manipulation (Register R/W) ===" << endl;

    uint32_t reg = 0;
    cout << "  initial reg       = 0x" << hex << setw(4) << setfill('0') << reg << endl;

    // Set bits
    reg |= (kStreamOnBit | kHdrEnableBit);
    cout << "  enable stream+hdr = 0x" << setw(4) << reg << endl;

    // Modify multi-bit field
    reg = set_field(reg, kGainShiftMask, kGainShiftPos, 5);
    cout << "  set gain_shift=5  = 0x" << setw(4) << reg << endl;

    // Extract multi-bit field
    uint32_t gain = get_field(reg, kGainShiftMask, kGainShiftPos);
    cout << dec << setfill(' ') << "  read gain_shift   = " << gain << endl;

    // Clear bit
    reg &= ~kHdrEnableBit;
    cout << "  disable hdr       = 0x" << hex << setw(4) << setfill('0') << reg
         << dec << setfill(' ') << endl;
    cout << endl;
}

// ============================================================
// 5. Endianness (Byte Order)
// ============================================================
// 本章重點：
//   ARM / x86 CPU 預設是 Little-Endian（低位元組放在低位址）。
//   但許多 I2C Image Sensor 暫存器與網路協定是 Big-Endian（高位元組在前）。
//   因此讀寫 16-bit / 32-bit 暫存器時常需要 byte swap。
constexpr uint16_t swap_bytes_u16(uint16_t v) {
    return static_cast<uint16_t>((v >> 8) | (v << 8));
}

void endianness_demo() {
    // Output:
    // === Endianness ===
    //   0x1234 swapped = 0x3412
    //
    cout << "=== Endianness ===" << endl;
    uint16_t sensor_reg = 0x1234;
    cout << "  0x1234 swapped = 0x" << hex << swap_bytes_u16(sensor_reg) << dec << endl;
    cout << endl;
}

int main() {
    fixed_width_demo();
    alignment_padding_demo();
    packed_and_alignas_demo();
    bit_manipulation_demo();
    endianness_demo();
    return 0;
}
