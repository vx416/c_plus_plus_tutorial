/**
 * Phase 12-4: HAL Asynchronous Session, ThreadPool EnqueueWork & ScopedTrace (ATRACE_CALL)
 *
 * 在 Pixel Camera HAL (`LyricHal`) 與 Android AIDL Camera HAL3 架構中，有三個重要的實戰慣例：
 *   1. 非同步 Request / Callback 模型：
 *      Framework 呼叫 `ProcessCaptureRequest(frame_number)` 時必須快速返回，不能在呼叫端阻塞等待硬體曝光。
 *      當硬體完成曝光與 ISP 處理後，HAL 再透過 `ICameraDeviceCallback` 非同步回呼：
 *        - `Notify(ShutterMsg{frame_number, timestamp_ns})`
 *        - `ProcessCaptureResult(CaptureResult{frame_number, metadata})`
 *   2. 使用受控的 `ThreadPool::EnqueueWork()`，禁止使用 `std::async`：
 *      `std::async` 在不同標準庫可能每次都開新 OS thread，無法控制執行緒數量、優先權與關機排空 (shutdown drain)。
 *   3. RAII `ScopedTrace` (`ATRACE_CALL()` / `ATRACE_NAME(name)`)：
 *      利用區域變數建構時寫入 `B|<pid>|<name>` (Begin)、離開 scope 解構時自動寫入 `E|<pid>` (End)，
 *      即使函式有多個 return 分支，Perfetto trace 區間也永遠成對閉合！
 */

#include <condition_variable>
#include <cstdint>
#include <functional>
#include <iostream>
#include <mutex>
#include <queue>
#include <string>
#include <string_view>
#include <thread>
#include <vector>
using namespace std;

// ============================================================
// 1. RAII ScopedTrace (模擬 Android ATRACE_CALL / Perfetto)
// ============================================================
class ScopedTrace {
public:
    explicit ScopedTrace(string_view name, vector<string>* trace_log)
        : name_(name), trace_log_(trace_log) {
        if (trace_log_) {
            trace_log_->push_back("B|" + name_);
        }
    }

    ~ScopedTrace() {
        if (trace_log_) {
            trace_log_->push_back("E|" + name_);
        }
    }

    ScopedTrace(const ScopedTrace&) = delete;
    ScopedTrace& operator=(const ScopedTrace&) = delete;

private:
    string name_;
    vector<string>* trace_log_;
};

#define HAL_ATRACE_NAME(name, log_ptr) ScopedTrace _scoped_trace_##__LINE__(name, log_ptr)
#define HAL_ATRACE_CALL(log_ptr) HAL_ATRACE_NAME(__func__, log_ptr)

// ============================================================
// 2. Bounded ThreadPool (EnqueueWork 取代 std::async)
// ============================================================
class HalThreadPool {
public:
    explicit HalThreadPool(size_t num_workers) {
        for (size_t i = 0; i < num_workers; ++i) {
            workers_.emplace_back([this] { WorkerLoop(); });
        }
    }

    ~HalThreadPool() {
        Shutdown();
    }

    void EnqueueWork(function<void()> work) {
        {
            lock_guard<mutex> lock(mu_);
            if (stopping_) return;
            queue_.push(std::move(work));
        }
        cv_.notify_one();
    }

    void Shutdown() {
        {
            lock_guard<mutex> lock(mu_);
            if (stopping_) return;
            stopping_ = true;
        }
        cv_.notify_all();
        for (thread& t : workers_) {
            if (t.joinable()) t.join();
        }
    }

private:
    void WorkerLoop() {
        while (true) {
            function<void()> task;
            {
                unique_lock<mutex> lock(mu_);
                cv_.wait(lock, [this] { return stopping_ || !queue_.empty(); });
                if (stopping_ && queue_.empty()) return;
                task = std::move(queue_.front());
                queue_.pop();
            }
            task();
        }
    }

    vector<thread> workers_;
    queue<function<void()>> queue_;
    mutex mu_;
    condition_variable cv_;
    bool stopping_ = false;
};

// ============================================================
// 3. Camera HAL3 Session & Asynchronous Callback Flow
// ============================================================
struct CaptureRequest {
    uint32_t frame_number;
    int32_t exposure_ms;
};

struct CaptureResult {
    uint32_t frame_number;
    uint64_t sensor_timestamp_ns;
    bool ok;
};

class ICameraDeviceCallback {
public:
    virtual ~ICameraDeviceCallback() = default;
    virtual void NotifyShutter(uint32_t frame_number, uint64_t timestamp_ns) = 0;
    virtual void ProcessCaptureResult(const CaptureResult& result) = 0;
};

class CameraDeviceSession {
public:
    CameraDeviceSession(HalThreadPool* pool, ICameraDeviceCallback* callback,
                        vector<string>* trace_log)
        : pool_(pool), callback_(callback), trace_log_(trace_log) {}

    void SubmitCaptureRequest(const CaptureRequest& req) {
        HAL_ATRACE_CALL(trace_log_);
        // 非同步排入 Session ThreadPool，不阻塞呼叫端
        pool_->EnqueueWork([this, req] {
            ProcessRequestOnWorker(req);
        });
    }

private:
    void ProcessRequestOnWorker(const CaptureRequest& req) {
        HAL_ATRACE_NAME("SensorReadoutAndIsp", trace_log_);
        uint64_t sof_ts = static_cast<uint64_t>(req.frame_number) * 33333333ULL;
        callback_->NotifyShutter(req.frame_number, sof_ts);

        CaptureResult res{req.frame_number, sof_ts, true};
        callback_->ProcessCaptureResult(res);
    }

    HalThreadPool* pool_;
    ICameraDeviceCallback* callback_;
    vector<string>* trace_log_;
};

class MockFrameworkCallback : public ICameraDeviceCallback {
public:
    void NotifyShutter(uint32_t frame_number, uint64_t timestamp_ns) override {
        lock_guard<mutex> lock(mu_);
        cout << "  [Callback] NotifyShutter frame=" << frame_number
             << " ts=" << timestamp_ns << endl;
    }

    void ProcessCaptureResult(const CaptureResult& result) override {
        lock_guard<mutex> lock(mu_);
        cout << "  [Callback] ProcessCaptureResult frame=" << result.frame_number
             << " ok=" << boolalpha << result.ok << noboolalpha << endl;
        ++completed_count_;
    }

    int completed_count() const {
        lock_guard<mutex> lock(mu_);
        return completed_count_;
    }

private:
    mutable mutex mu_;
    int completed_count_ = 0;
};

int main() {
    // Output:
    // === HAL Async Session, ThreadPool & ScopedTrace ===
    //   [Callback] NotifyShutter frame=1 ts=33333333
    //   [Callback] ProcessCaptureResult frame=1 ok=true
    //   trace events recorded = 4
    //   trace[0] = B|SubmitCaptureRequest
    //   trace[1] = E|SubmitCaptureRequest
    //   trace[2] = B|SensorReadoutAndIsp
    //   trace[3] = E|SensorReadoutAndIsp
    //
    cout << "=== HAL Async Session, ThreadPool & ScopedTrace ===" << endl;

    vector<string> trace_log;
    MockFrameworkCallback callback;
    {
        HalThreadPool pool(1);
        CameraDeviceSession session(&pool, &callback, &trace_log);
        session.SubmitCaptureRequest(CaptureRequest{1, 16});
        pool.Shutdown();
    }

    cout << "  trace events recorded = " << trace_log.size() << endl;
    for (size_t i = 0; i < trace_log.size(); ++i) {
        cout << "  trace[" << i << "] = " << trace_log[i] << endl;
    }
    cout << endl;
    return 0;
}
