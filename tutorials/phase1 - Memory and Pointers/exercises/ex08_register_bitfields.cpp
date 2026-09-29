/**
 * 練習 8: Register Bitfields & Struct Layout
 *
 * 模擬影像感測器 (Image Sensor) 的 16-bit 控制暫存器操作：
 *   Bit [0]     : STREAM_ENABLE (0 = standby, 1 = streaming)
 *   Bit [3:1]   : FPS_MODE      (3 bits, 0~7)
 *   Bit [11:4]  : DIGITAL_GAIN  (8 bits, 0~255)
 *   Bit [15]    : TEST_PATTERN  (1 bit)
 *
 * 請完成以下 helper 函式並通過所有 assert。
 */

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <iostream>
using namespace std;

constexpr uint16_t kStreamEnableMask = (1u << 0);
constexpr uint16_t kFpsModeShift = 1;
constexpr uint16_t kFpsModeMask = (0x7u << kFpsModeShift);
constexpr uint16_t kDigitalGainShift = 4;
constexpr uint16_t kDigitalGainMask = (0xFFu << kDigitalGainShift);
constexpr uint16_t kTestPatternMask = (1u << 15);

// 設定或清除 STREAM_ENABLE bit
uint16_t set_stream_enable(uint16_t reg, bool enable) {
    if (enable) {
        return static_cast<uint16_t>(reg | kStreamEnableMask);
    }
    return static_cast<uint16_t>(reg & ~kStreamEnableMask);
}

// 寫入 FPS_MODE (3 bits)
uint16_t set_fps_mode(uint16_t reg, uint8_t mode) {
    uint16_t cleared = static_cast<uint16_t>(reg & ~kFpsModeMask);
    uint16_t shifted = static_cast<uint16_t>((static_cast<uint16_t>(mode) << kFpsModeShift) & kFpsModeMask);
    return static_cast<uint16_t>(cleared | shifted);
}

// 讀取 FPS_MODE (3 bits)
uint8_t get_fps_mode(uint16_t reg) {
    return static_cast<uint8_t>((reg & kFpsModeMask) >> kFpsModeShift);
}

// 寫入 DIGITAL_GAIN (8 bits)
uint16_t set_digital_gain(uint16_t reg, uint8_t gain) {
    uint16_t cleared = static_cast<uint16_t>(reg & ~kDigitalGainMask);
    uint16_t shifted = static_cast<uint16_t>((static_cast<uint16_t>(gain) << kDigitalGainShift) & kDigitalGainMask);
    return static_cast<uint16_t>(cleared | shifted);
}

// 讀取 DIGITAL_GAIN (8 bits)
uint8_t get_digital_gain(uint16_t reg) {
    return static_cast<uint8_t>((reg & kDigitalGainMask) >> kDigitalGainShift);
}

// 重新排列成員順序，使 CompactPacket 在 64-bit 系統上 sizeof 為 16 bytes
struct CompactPacket {
    uint64_t timestamp_ns;
    uint32_t frame_number;
    uint16_t sensor_id;
    uint8_t status;
    uint8_t flags;
};

int main() {
    uint16_t reg = 0x0000;

    reg = set_stream_enable(reg, true);
    assert((reg & kStreamEnableMask) != 0);

    reg = set_fps_mode(reg, 5);
    assert(get_fps_mode(reg) == 5);
    assert((reg & kStreamEnableMask) != 0);  // 不影響原本的 bit

    reg = set_digital_gain(reg, 0xA5);
    assert(get_digital_gain(reg) == 0xA5);
    assert(get_fps_mode(reg) == 5);

    // 覆寫 fps_mode 不應破壞 digital_gain
    reg = set_fps_mode(reg, 2);
    assert(get_fps_mode(reg) == 2);
    assert(get_digital_gain(reg) == 0xA5);

    reg = set_stream_enable(reg, false);
    assert((reg & kStreamEnableMask) == 0);
    assert(get_digital_gain(reg) == 0xA5);

    static_assert(sizeof(CompactPacket) == 16, "CompactPacket should be 16 bytes without padding waste");

    cout << "ex08 passed!" << endl;
    return 0;
}
