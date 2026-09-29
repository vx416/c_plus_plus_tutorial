/**
 * Phase 13-3: Kernel & Driver Boundary Debugging (MMIO `volatile`, `D`-State Hang, Ring Buffer Flight Recorder)
 *
 * 當你在開發或除錯與 Linux Kernel Driver（如 V4L2、LWIS、I2C Sensor Driver、DMA）互動的系統程式時，
 * 常會遇到三種跟純 User-space 軟體完全不同的問題：
 *
 *   1. `volatile` 的真正用途（MMIO 硬體暫存器 vs 多執行緒）：
 *      - `volatile` **不能**用來做多執行緒同步（它不提供 atomicity，也不提供 CPU Memory Barrier，那是用 `std::atomic` 做的）。
 *      - `volatile` 的真正用途是 **Memory-Mapped I/O (MMIO)**：告訴編譯器「這個位址背後連著硬體暫存器，
 *        每次讀寫都有硬體副作用，絕對不可以把連續兩次寫入合併、也不可以把 polling 迴圈優化掉」！
 *   2. Uninterruptible Sleep (`D` State) 與 I2C / IRQ Timeout：
 *      - 在 Kernel Driver 中，若用 `mutex_lock()` 或 `wait_event()` 死等硬體中斷（而不是 `wait_event_timeout()`），
 *        一旦硬體沒給電 (Power rail off) 或 MIPI/I2C 卡死，執行緒會進入 `D` state（連 `kill -9` 都殺不死），
 *        最終觸發 Kernel `hung_task_timeout_secs` panic 或 HAL Watchdog abort。
 *   3. 零動態配置的 Crash Flight Recorder (Ring Buffer)：
 *      - 如果每一幀、每次寄存器讀寫都印 `ALOGI` / `printk`，I/O 開銷會直接害死即時性（Heisenbug：加了 log 時序變了，bug 就不見或更嚴重）。
 *      - 實務做法：平時把高頻事件寫入記憶體內的固定大小 **Ring Buffer**（幾十奈秒、零 heap allocation），
 *        只有在發生 Timeout / Error / Crash 時才把最後 N 筆歷史一口氣印出來！
 */

#include <array>
#include <atomic>
#include <cstdint>
#include <iostream>
#include <string_view>
using namespace std;

// ============================================================
// 1. MMIO Register Polling：`volatile` 與 Timeout 防呆
// ============================================================
// 本章重點：
//   如果沒有 `volatile`，編譯器在 -O2 下看到：
//     while ((*status_reg & kReadyBit) == 0) {}
//   會以為 `*status_reg` 在迴圈內根本沒被程式修改，於是只讀一次暫存器就變成無窮迴圈！
//   加上 `volatile const uint32_t*` 後，編譯器每次迭代都強制發出真正的記憶體讀取指令 (`ldr`)。
constexpr uint32_t kSensorReadyBit = (1u << 0);
constexpr int kErrTimedOut = -110;  // Linux -ETIMEDOUT

int poll_register_with_timeout(volatile const uint32_t* status_reg, int max_retries) {
    for (int attempt = 0; attempt < max_retries; ++attempt) {
        if ((*status_reg & kSensorReadyBit) != 0) {
            return 0;  // 0 = OK
        }
    }
    // 永遠要有上限，回傳 -ETIMEDOUT，絕對不能無上限死等導致 D-state hang！
    return kErrTimedOut;
}

void mmio_polling_demo() {
    // Output:
    // === MMIO Register Polling (volatile + timeout) ===
    //   when hw ready bit is 1 -> ret = 0 (OK)
    //   when hw stuck at 0     -> ret = -110 (-ETIMEDOUT, avoids infinite D-state hang)
    //
    cout << "=== MMIO Register Polling (volatile + timeout) ===" << endl;

    volatile uint32_t fake_hw_reg = kSensorReadyBit;
    cout << "  when hw ready bit is 1 -> ret = "
         << poll_register_with_timeout(&fake_hw_reg, 10) << " (OK)" << endl;

    fake_hw_reg = 0;  // 模擬硬體沒給 clock 或 I2C NACK 卡住
    cout << "  when hw stuck at 0     -> ret = "
         << poll_register_with_timeout(&fake_hw_reg, 10)
         << " (-ETIMEDOUT, avoids infinite D-state hang)" << endl << endl;
}

// ============================================================
// 2. Crash Flight Recorder (固定大小、無鎖/無動態配置 Ring Buffer)
// ============================================================
// 本章重點：
//   類似 Linux Kernel `ftrace` ring buffer 或驅動內部的 History Log：
//   - 容量固定為 2 的次方（例如 N = 4），用 `index & (N - 1)` 取代除法取餘數。
//   - 每次紀錄只寫入固定大小的 struct，不配置 `std::string`，可在 ISR / 高頻路徑安全使用。
struct FlightEvent {
    uint64_t timestamp_us = 0;
    const char* tag = "";
    uint32_t reg_addr = 0;
    uint32_t reg_val = 0;
};

template <size_t CapacityPow2>
class FlightRecorder {
    static_assert((CapacityPow2 & (CapacityPow2 - 1)) == 0,
                  "CapacityPow2 must be a power of 2");

public:
    void Record(uint64_t ts_us, const char* tag, uint32_t addr, uint32_t val) {
        size_t idx = write_seq_.fetch_add(1, memory_order_relaxed);
        buffer_[idx & (CapacityPow2 - 1)] = FlightEvent{ts_us, tag, addr, val};
    }

    void DumpLastEvents() const {
        size_t total = write_seq_.load(memory_order_relaxed);
        size_t count = (total < CapacityPow2) ? total : CapacityPow2;
        size_t start = total - count;

        cout << "  --- Flight Recorder Dump (last " << count
             << " of " << total << " events) ---" << endl;
        for (size_t i = start; i < total; ++i) {
            const auto& ev = buffer_[i & (CapacityPow2 - 1)];
            cout << "    [t=" << ev.timestamp_us << "us] " << ev.tag
                 << " reg=0x" << hex << ev.reg_addr
                 << " val=0x" << ev.reg_val << dec << endl;
        }
    }

private:
    array<FlightEvent, CapacityPow2> buffer_{};
    atomic<size_t> write_seq_{0};
};

void flight_recorder_demo() {
    // Output:
    // === Kernel / HAL Flight Recorder (Ring Buffer) ===
    //   --- Flight Recorder Dump (last 4 of 6 events) ---
    //     [t=1030us] I2C_WRITE reg=0x100 val=0x1
    //     [t=1040us] IRQ_SOF reg=0x0 val=0x2a
    //     [t=1050us] I2C_WRITE reg=0x202 val=0x640
    //     [t=5050us] WATCHDOG_TIMEOUT reg=0xffff val=0x110
    //
    cout << "=== Kernel / HAL Flight Recorder (Ring Buffer) ===" << endl;

    FlightRecorder<4> recorder;
    recorder.Record(1000, "POWER_ON", 0x0001, 0x01);
    recorder.Record(1010, "PLL_LOCK", 0x0002, 0x01);
    recorder.Record(1030, "I2C_WRITE", 0x0100, 0x01);
    recorder.Record(1040, "IRQ_SOF", 0x0000, 42);
    recorder.Record(1050, "I2C_WRITE", 0x0202, 1600);
    recorder.Record(5050, "WATCHDOG_TIMEOUT", 0xFFFF, 0x110);

    // 只保留最後 4 筆關鍵現場，最舊的 POWER_ON 與 PLL_LOCK 被自動覆蓋
    recorder.DumpLastEvents();
    cout << endl;
}

// ============================================================
// 3. Linux Kernel Debug 工具鏈速查
// ============================================================
void kernel_debug_tools_summary() {
    // Output:
    // === Linux Kernel / Driver Debug Interfaces ===
    //   dmesg / pstore    : kernel log & console-ramoops across sudden reboot/panic
    //   /sys/kernel/debug : debugfs (regmap dump, clk_summary, gpio, regulator status)
    //   ftrace / kprobes  : trace kernel functions (e.g. i2c_transfer, v4l2_ioctl) without rebuilding
    //   CONFIG_LOCKDEP    : runtime kernel lock dependency validator (detects AB-BA deadlock early)
    //
    cout << "=== Linux Kernel / Driver Debug Interfaces ===" << endl;
    cout << "  dmesg / pstore    : kernel log & console-ramoops across sudden reboot/panic" << endl;
    cout << "  /sys/kernel/debug : debugfs (regmap dump, clk_summary, gpio, regulator status)" << endl;
    cout << "  ftrace / kprobes  : trace kernel functions (e.g. i2c_transfer, v4l2_ioctl) without rebuilding" << endl;
    cout << "  CONFIG_LOCKDEP    : runtime kernel lock dependency validator (detects AB-BA deadlock early)" << endl;
    cout << endl;
}

int main() {
    mmio_polling_demo();
    flight_recorder_demo();
    kernel_debug_tools_summary();
    return 0;
}
