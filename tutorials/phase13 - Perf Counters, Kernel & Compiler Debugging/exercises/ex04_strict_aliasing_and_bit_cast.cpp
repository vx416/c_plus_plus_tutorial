/**
 * 練習 4: 用 `std::bit_cast` 取代違反 Strict Aliasing 的 `reinterpret_cast`
 *
 * 1. 實作 FloatToRawBits(float f) 與 RawBitsToFloat(uint32_t bits)
 * 2. 實作 ExtractFloatExponent(float f)：
 *    IEEE-754 single-precision float 格式：
 *      Bit [31]    : Sign (1 bit)
 *      Bit [30:23] : Exponent (8 bits)
 *      Bit [22:0]  : Mantissa (23 bits)
 */

#include <bit>
#include <cassert>
#include <cstdint>
#include <iostream>
using namespace std;

uint32_t FloatToRawBits(float f) {
    return std::bit_cast<uint32_t>(f);
}

float RawBitsToFloat(uint32_t bits) {
    return std::bit_cast<float>(bits);
}

uint8_t ExtractFloatExponent(float f) {
    uint32_t bits = std::bit_cast<uint32_t>(f);
    return static_cast<uint8_t>((bits >> 23) & 0xFFu);
}

int main() {
    assert(FloatToRawBits(1.0f) == 0x3f800000u);
    assert(RawBitsToFloat(0x40000000u) == 2.0f);

    // 1.0f 的 biased exponent 為 127 (0x7F)，2.0f 為 128 (0x80)
    assert(ExtractFloatExponent(1.0f) == 127);
    assert(ExtractFloatExponent(2.0f) == 128);

    cout << "ex04 passed!" << endl;
    return 0;
}
