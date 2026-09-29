/**
 * Phase 6-7: Clang Thread Safety Annotations (GUARDED_BY / LOCKS_EXCLUDED)
 *
 * 在大型 C++ 專案（如 Android AOSP libbase、Pixel Camera HAL LyricHal、Abseil）中，
 * 光靠人工 code review 很難記住「哪個變數受哪把 mutex 保護」、「哪個 helper 函式呼叫前已經拿過鎖」。
 *
 * Clang 提供了編譯期靜態分析 (-Wthread-safety)：
 *   - GUARDED_BY(mu_)               宣告該成員變數讀寫時必須持有 mu_
 *   - PT_GUARDED_BY(mu_)            指標本身不鎖，但解參考 (*ptr) 指到的資料受 mu_ 保護
 *   - EXCLUSIVE_LOCKS_REQUIRED(mu_) 呼叫此函式前，呼叫端必須已經持有 mu_（常見於 *Locked() helper）
 *   - LOCKS_EXCLUDED(mu_)           呼叫此函式前，呼叫端不可持有 mu_（防止同執行緒重複上鎖 deadlock）
 *
 * 這些標註完全是「編譯期零成本 (zero runtime overhead)」，編譯完不留任何額外指令。
 *
 * 目錄:
 *   1. Thread Safety Annotation 巨集定義原理
 *   2. GUARDED_BY 保護成員變數
 *   3. Public API (LOCKS_EXCLUDED) vs Private *Locked() (EXCLUSIVE_LOCKS_REQUIRED)
 *   4. 為什麼這能同時防止 Data Race 與 Self-Deadlock
 */

#include <iostream>
#include <mutex>
#include <string>
#include <vector>
using namespace std;

// ============================================================
// 1. Thread Safety Annotation 巨集定義原理
// ============================================================
// 在 Clang 下展開為 __attribute__((...))，在非 Clang 編譯器退化為空巨集。
// AOSP 對應標頭：<android-base/thread_annotations.h>
// Abseil 對應標頭：ABSL_GUARDED_BY / ABSL_EXCLUSIVE_LOCKS_REQUIRED / ABSL_LOCKS_EXCLUDED
#if defined(__clang__)
#define THREAD_ANNOTATION_ATTRIBUTE__(x) __attribute__((x))
#else
#define THREAD_ANNOTATION_ATTRIBUTE__(x)
#endif

#define CAPABILITY(x) THREAD_ANNOTATION_ATTRIBUTE__(capability(x))
#define SCOPED_CAPABILITY THREAD_ANNOTATION_ATTRIBUTE__(scoped_lockable)
#define GUARDED_BY(x) THREAD_ANNOTATION_ATTRIBUTE__(guarded_by(x))
#define PT_GUARDED_BY(x) THREAD_ANNOTATION_ATTRIBUTE__(pt_guarded_by(x))
#define EXCLUSIVE_LOCKS_REQUIRED(...) \
    THREAD_ANNOTATION_ATTRIBUTE__(requires_capability(__VA_ARGS__))
#define LOCKS_EXCLUDED(...) \
    THREAD_ANNOTATION_ATTRIBUTE__(locks_excluded(__VA_ARGS__))
#define ACQUIRE(...) \
    THREAD_ANNOTATION_ATTRIBUTE__(acquire_capability(__VA_ARGS__))
#define RELEASE(...) \
    THREAD_ANNOTATION_ATTRIBUTE__(release_capability(__VA_ARGS__))

// 薄封裝 std::mutex，掛上 CAPABILITY("mutex") 讓 Clang 認識這把鎖
class CAPABILITY("mutex") AnnotatedMutex {
public:
    void lock() ACQUIRE() { mu_.lock(); }
    void unlock() RELEASE() { mu_.unlock(); }
    std::mutex& native() { return mu_; }

private:
    std::mutex mu_;
};

// 薄封裝 RAII Lock Guard，掛上 SCOPED_CAPABILITY
class SCOPED_CAPABILITY AnnotatedLock {
public:
    explicit AnnotatedLock(AnnotatedMutex& mu) ACQUIRE(mu) : mu_(mu) {
        mu_.lock();
    }
    ~AnnotatedLock() RELEASE() {
        mu_.unlock();
    }
    AnnotatedLock(const AnnotatedLock&) = delete;
    AnnotatedLock& operator=(const AnnotatedLock&) = delete;

private:
    AnnotatedMutex& mu_;
};

// ============================================================
// 2. GUARDED_BY 與 *Locked() 慣用法
// ============================================================
// 本章重點：
//   在 Android / HAL 中，一個 class 若有內部 mutex_，常見規範是：
//     1. 所有受保護的成員變數一律加上 GUARDED_BY(mutex_)。
//     2. 對外公開的 public 方法標註 LOCKS_EXCLUDED(mutex_)，負責在入口處拿鎖。
//     3. 內部共用的 private helper 命名為 XxxLocked()，標註 EXCLUSIVE_LOCKS_REQUIRED(mutex_)，
//        內部不再重複拿鎖，直接存取 GUARDED_BY 變數。
class SensorStreamController {
public:
    void StartStream(int fps) LOCKS_EXCLUDED(mutex_) {
        AnnotatedLock lock(mutex_);
        fps_ = fps;
        streaming_ = true;
        RecomputeExposureLocked();  // OK：已經持有 mutex_
    }

    void UpdateFps(int fps) LOCKS_EXCLUDED(mutex_) {
        AnnotatedLock lock(mutex_);
        fps_ = fps;
        if (streaming_) {
            RecomputeExposureLocked();  // OK：共用內部邏輯，且不會二次 lock 造成 deadlock
        }
    }

    int GetMaxExposureLines() const LOCKS_EXCLUDED(mutex_) {
        AnnotatedLock lock(mutex_);
        // 如果忘記寫上面那行 AnnotatedLock，且開啟 -Wthread-safety，
        // 編譯器會在這裡直接報錯：reading variable 'max_exposure_lines_' requires holding capability 'mutex_'
        return max_exposure_lines_;
    }

private:
    // 要求呼叫者必須已經拿著 mutex_
    void RecomputeExposureLocked() EXCLUSIVE_LOCKS_REQUIRED(mutex_) {
        // 如果在這裡不小心呼叫 StartStream(30)，因為 StartStream 標了 LOCKS_EXCLUDED(mutex_)，
        // 編譯器會直接報錯，幫你在編譯期攔截 self-deadlock！
        max_exposure_lines_ = (fps_ > 0) ? (100000 / fps_) : 0;
    }

    mutable AnnotatedMutex mutex_;
    bool streaming_ GUARDED_BY(mutex_) = false;
    int fps_ GUARDED_BY(mutex_) = 30;
    int max_exposure_lines_ GUARDED_BY(mutex_) = 0;
};

void guarded_by_demo() {
    // Output:
    // === Clang Thread Safety Annotations ===
    //   StartStream(30) -> max_exposure_lines = 3333
    //   UpdateFps(60)   -> max_exposure_lines = 1666
    //
    cout << "=== Clang Thread Safety Annotations ===" << endl;

    SensorStreamController controller;
    controller.StartStream(30);
    cout << "  StartStream(30) -> max_exposure_lines = "
         << controller.GetMaxExposureLines() << endl;

    controller.UpdateFps(60);
    cout << "  UpdateFps(60)   -> max_exposure_lines = "
         << controller.GetMaxExposureLines() << endl;
    cout << endl;
}

// ============================================================
// 3. 總結對照表
// ============================================================
void summary_demo() {
    // Output:
    // === Thread Safety Summary ===
    //   GUARDED_BY(mu_)               protect data member
    //   LOCKS_EXCLUDED(mu_)           public method acquires lock itself
    //   EXCLUSIVE_LOCKS_REQUIRED(mu_) private *Locked() helper assumes lock held
    //
    cout << "=== Thread Safety Summary ===" << endl;
    cout << "  GUARDED_BY(mu_)               protect data member" << endl;
    cout << "  LOCKS_EXCLUDED(mu_)           public method acquires lock itself" << endl;
    cout << "  EXCLUSIVE_LOCKS_REQUIRED(mu_) private *Locked() helper assumes lock held" << endl;
    cout << endl;
}

int main() {
    guarded_by_demo();
    summary_demo();
    return 0;
}
