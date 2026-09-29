/**
 * Phase 13-1: Perfetto / Ftrace (`trace_marker`) — Sync Slice, Async Slice & Counter Tracks
 *
 * 在 Android 與 Linux 系統層除錯中，Perfetto (與底層 Linux ftrace `/sys/kernel/tracing/trace_marker`)
 * 是分析掉幀 (Frame Drop)、執行緒卡頓與硬體排程的標準工具。
 *
 * User-space 程式寫入 `trace_marker` 的文字協定有四種核心格式：
 *   1. Sync Slice (同執行緒 Call Stack 區間):
 *      - `B|<pid>|<slice_name>`  (Begin，對標 ATRACE_BEGIN / ATRACE_CALL)
 *      - `E|<pid>`               (End，對標 ATRACE_END，自動配對同執行緒最後一個 B)
 *   2. Async Slice (跨執行緒非同步生命週期，帶整數 cookie):
 *      - `S|<pid>|<slice_name>|<cookie>` (Async Start，對標 ATRACE_ASYNC_BEGIN)
 *      - `F|<pid>|<slice_name>|<cookie>` (Async Finish，對標 ATRACE_ASYNC_END)
 *      * 為什麼 Camera HAL 必備？
 *        因為 `SubmitRequest(frame=42)` 在 Binder Thread，而 `NotifyShutter(frame=42)` 在 Sensor/ISP Thread！
 *        用 Sync Slice (`B`/`E`) 跨執行緒會導致時間軸錯亂，必須用 `cookie = frame_number` 的 `S`/`F`！
 *   3. Counter Track (隨時間變化的數值折線/階梯圖):
 *      - `C|<pid>|<counter_name>|<value>` (對標 ATRACE_INT / ATRACE_INT64)
 *      * 常用於觀察：`InFlightRequests`、`FreeDmaBufCount`、`SensorExposureUs`、`ThermalTempmC`。
 *   4. Instant Event (單一時間點事件):
 *      - `I|<pid>|<event_name>` (例如 VSync tick、SOF interrupt 觸發點)
 */

#include <cstdint>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>
using namespace std;

class TraceMarkerSink {
public:
    explicit TraceMarkerSink(int pid) : pid_(pid) {}

    // 1. Sync Slice: B|<pid>|<name> & E|<pid>
    void BeginSync(string_view name) {
        events_.push_back("B|" + to_string(pid_) + "|" + string(name));
    }

    void EndSync() {
        events_.push_back("E|" + to_string(pid_));
    }

    // 2. Async Slice: S|<pid>|<name>|<cookie> & F|<pid>|<name>|<cookie>
    void BeginAsync(string_view name, int64_t cookie) {
        events_.push_back("S|" + to_string(pid_) + "|" + string(name) + "|" + to_string(cookie));
    }

    void EndAsync(string_view name, int64_t cookie) {
        events_.push_back("F|" + to_string(pid_) + "|" + string(name) + "|" + to_string(cookie));
    }

    // 3. Counter Track: C|<pid>|<name>|<value>
    void TraceCounter(string_view counter_name, int64_t value) {
        events_.push_back("C|" + to_string(pid_) + "|" + string(counter_name) + "|" + to_string(value));
    }

    // 4. Instant Event: I|<pid>|<name>
    void TraceInstant(string_view event_name) {
        events_.push_back("I|" + to_string(pid_) + "|" + string(event_name));
    }

    const vector<string>& events() const { return events_; }

private:
    int pid_;
    vector<string> events_;
};

// RAII Wrapper for Sync Slice
class ScopedSyncTrace {
public:
    ScopedSyncTrace(TraceMarkerSink& sink, string_view name) : sink_(sink) {
        sink_.BeginSync(name);
    }
    ~ScopedSyncTrace() {
        sink_.EndSync();
    }

private:
    TraceMarkerSink& sink_;
};

void perfetto_camera_frame_demo() {
    // Output:
    // === Perfetto trace_marker Protocol Demo ===
    //   B|1042|ProcessCaptureRequest
    //   S|1042|FrameLifecycle|101
    //   C|1042|InFlightRequests|1
    //   C|1042|SensorExposureUs|16666
    //   E|1042
    //   I|1042|VSYNC_SOF_IRQ
    //   B|1042|IspWorkerProcess
    //   E|1042
    //   F|1042|FrameLifecycle|101
    //   C|1042|InFlightRequests|0
    //
    cout << "=== Perfetto trace_marker Protocol Demo ===" << endl;

    TraceMarkerSink tracer(/*pid=*/1042);
    const int64_t frame_num = 101;

    // [Thread 1: Binder Thread] 收到拍第 101 幀的請求
    {
        ScopedSyncTrace sync_slice(tracer, "ProcessCaptureRequest");
        tracer.BeginAsync("FrameLifecycle", frame_num);
        tracer.TraceCounter("InFlightRequests", 1);
        tracer.TraceCounter("SensorExposureUs", 16666);
    }

    // [Hardware Interrupt] Sensor 發出 SOF (Start of Frame)
    tracer.TraceInstant("VSYNC_SOF_IRQ");

    // [Thread 2: ISP Worker Thread] 完成影像處理並回傳結果，結束跨執行緒的 FrameLifecycle(101)
    {
        ScopedSyncTrace sync_slice(tracer, "IspWorkerProcess");
    }
    tracer.EndAsync("FrameLifecycle", frame_num);
    tracer.TraceCounter("InFlightRequests", 0);

    for (const string& line : tracer.events()) {
        cout << "  " << line << endl;
    }
    cout << endl;
}

int main() {
    perfetto_camera_frame_demo();
    return 0;
}
