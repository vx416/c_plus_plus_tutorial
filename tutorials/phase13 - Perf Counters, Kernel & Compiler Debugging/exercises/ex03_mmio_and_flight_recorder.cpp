/**
 * 練習 3: MMIO `volatile` Polling Timeout 與固定大小 Flight Recorder Ring Buffer
 *
 * 1. 實作 PollBitSet(reg, mask, max_attempts)：若在 max_attempts 次內 `( *reg & mask ) != 0` 回傳 true，
 *    否則回傳 false（避免無窮迴圈 D-state hang）。
 * 2. 實作 RingLog<N>：只保留最後 N 筆紀錄，並依時間先後順序回傳 Snapshot()。
 */

#include <array>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <vector>
using namespace std;

bool PollBitSet(volatile const uint32_t* reg, uint32_t mask, int max_attempts) {
    for (int i = 0; i < max_attempts; ++i) {
        if ((*reg & mask) != 0) {
            return true;
        }
    }
    return false;
}

template <size_t N>
class RingLog {
    static_assert((N & (N - 1)) == 0, "N must be a power of 2");

public:
    void Push(uint32_t code) {
        buf_[write_count_ & (N - 1)] = code;
        ++write_count_;
    }

    vector<uint32_t> Snapshot() const {
        size_t count = (write_count_ < N) ? write_count_ : N;
        size_t start = write_count_ - count;
        vector<uint32_t> out;
        out.reserve(count);
        for (size_t i = start; i < write_count_; ++i) {
            out.push_back(buf_[i & (N - 1)]);
        }
        return out;
    }

private:
    array<uint32_t, N> buf_{};
    size_t write_count_ = 0;
};

int main() {
    volatile uint32_t hw_status = 0x04;
    assert(PollBitSet(&hw_status, 0x04, 5));
    assert(!PollBitSet(&hw_status, 0x01, 5));

    RingLog<4> log;
    log.Push(10);
    log.Push(20);
    assert((log.Snapshot() == vector<uint32_t>{10, 20}));

    log.Push(30);
    log.Push(40);
    log.Push(50);
    log.Push(60);
    // 容量為 4，最舊的 10, 20 被覆蓋，順序應為 30, 40, 50, 60
    assert((log.Snapshot() == vector<uint32_t>{30, 40, 50, 60}));

    cout << "ex03 passed!" << endl;
    return 0;
}
