/**
 * 練習 6: std::span 與 StrongId
 *
 * 1. 實作 StrongId<Tag, T> 讓 StreamId 與 BufferId 型別隔離並支援 map 查找。
 * 2. 實作 parse_packet_payload(span<const uint8_t> packet)：
 *    假設封包前 2 bytes 是 header，最後 1 byte 是 footer，
 *    請回傳中間的 payload 切片（若總長度 < 3 則回傳空 span）。
 */

#include <array>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <map>
#include <span>
#include <vector>
using namespace std;

template <typename Tag, typename T = int>
class StrongId {
public:
    constexpr explicit StrongId(T v) : value_(v) {}
    constexpr T value() const { return value_; }

    friend constexpr bool operator==(StrongId a, StrongId b) { return a.value_ == b.value_; }
    friend constexpr bool operator!=(StrongId a, StrongId b) { return a.value_ != b.value_; }
    friend constexpr bool operator<(StrongId a, StrongId b) { return a.value_ < b.value_; }

private:
    T value_;
};

struct StreamIdTag {};
struct BufferIdTag {};

using StreamId = StrongId<StreamIdTag, int>;
using BufferId = StrongId<BufferIdTag, int>;

span<const uint8_t> parse_packet_payload(span<const uint8_t> packet) {
    if (packet.size() < 3) {
        return {};
    }
    return packet.subspan(2, packet.size() - 3);
}

int main() {
    StreamId s1(1);
    StreamId s2(2);
    BufferId b1(1);

    assert(s1 == StreamId(1));
    assert(s1 != s2);
    assert(s1 < s2);
    assert(s1.value() == b1.value());  // 底層數值相等，但 StreamId 與 BufferId 不能直接 ==

    map<StreamId, string> stream_names;
    stream_names[s1] = "preview";
    stream_names[s2] = "video";
    assert(stream_names[StreamId(1)] == "preview");

    vector<uint8_t> pkt{0xAA, 0xBB, 10, 20, 30, 0xFF};
    span<const uint8_t> payload = parse_packet_payload(pkt);
    assert(payload.size() == 3);
    assert(payload[0] == 10 && payload[1] == 20 && payload[2] == 30);

    array<uint8_t, 2> short_pkt{0xAA, 0xBB};
    assert(parse_packet_payload(short_pkt).empty());

    cout << "ex06 passed!" << endl;
    return 0;
}
