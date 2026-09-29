/**
 * 練習 7: Thread Safety Annotations 與 *Locked() 模式
 *
 * 請完成 FrameQueueManager：
 *   - 成員變數 pending_frames_ 與 dropped_count_ 皆受 mutex_ 保護 (GUARDED_BY)
 *   - Public API (PushFrame, PopFrame, GetDroppedCount) 標註 LOCKS_EXCLUDED(mutex_)
 *   - 當佇列超過 max_depth_ 時，PushFrame 會呼叫私有的 DropOldestLocked()
 *     （標註 EXCLUSIVE_LOCKS_REQUIRED(mutex_)）丟棄最舊的一筆並遞增 dropped_count_。
 */

#include <cassert>
#include <deque>
#include <iostream>
#include <mutex>
#include <optional>
using namespace std;

#if defined(__clang__)
#define THREAD_ANNOTATION_ATTRIBUTE__(x) __attribute__((x))
#else
#define THREAD_ANNOTATION_ATTRIBUTE__(x)
#endif

#define CAPABILITY(x) THREAD_ANNOTATION_ATTRIBUTE__(capability(x))
#define SCOPED_CAPABILITY THREAD_ANNOTATION_ATTRIBUTE__(scoped_lockable)
#define GUARDED_BY(x) THREAD_ANNOTATION_ATTRIBUTE__(guarded_by(x))
#define EXCLUSIVE_LOCKS_REQUIRED(...) \
    THREAD_ANNOTATION_ATTRIBUTE__(requires_capability(__VA_ARGS__))
#define LOCKS_EXCLUDED(...) \
    THREAD_ANNOTATION_ATTRIBUTE__(locks_excluded(__VA_ARGS__))
#define ACQUIRE(...) \
    THREAD_ANNOTATION_ATTRIBUTE__(acquire_capability(__VA_ARGS__))
#define RELEASE(...) \
    THREAD_ANNOTATION_ATTRIBUTE__(release_capability(__VA_ARGS__))

class CAPABILITY("mutex") AnnotatedMutex {
public:
    void lock() ACQUIRE() { mu_.lock(); }
    void unlock() RELEASE() { mu_.unlock(); }

private:
    std::mutex mu_;
};

class SCOPED_CAPABILITY AnnotatedLock {
public:
    explicit AnnotatedLock(AnnotatedMutex& mu) ACQUIRE(mu) : mu_(mu) { mu_.lock(); }
    ~AnnotatedLock() RELEASE() { mu_.unlock(); }

private:
    AnnotatedMutex& mu_;
};

class FrameQueueManager {
public:
    explicit FrameQueueManager(size_t max_depth) : max_depth_(max_depth) {}

    void PushFrame(int frame_number) LOCKS_EXCLUDED(mutex_) {
        AnnotatedLock lock(mutex_);
        if (pending_frames_.size() >= max_depth_) {
            DropOldestLocked();
        }
        pending_frames_.push_back(frame_number);
    }

    optional<int> PopFrame() LOCKS_EXCLUDED(mutex_) {
        AnnotatedLock lock(mutex_);
        if (pending_frames_.empty()) {
            return nullopt;
        }
        int front = pending_frames_.front();
        pending_frames_.pop_front();
        return front;
    }

    size_t GetDroppedCount() const LOCKS_EXCLUDED(mutex_) {
        AnnotatedLock lock(mutex_);
        return dropped_count_;
    }

private:
    void DropOldestLocked() EXCLUSIVE_LOCKS_REQUIRED(mutex_) {
        if (!pending_frames_.empty()) {
            pending_frames_.pop_front();
            ++dropped_count_;
        }
    }

    const size_t max_depth_;
    mutable AnnotatedMutex mutex_;
    deque<int> pending_frames_ GUARDED_BY(mutex_);
    size_t dropped_count_ GUARDED_BY(mutex_) = 0;
};

int main() {
    FrameQueueManager q(2);

    q.PushFrame(101);
    q.PushFrame(102);
    assert(q.GetDroppedCount() == 0);

    // 超過容量 2，應該觸發 DropOldestLocked() 丟掉 101
    q.PushFrame(103);
    assert(q.GetDroppedCount() == 1);

    auto f1 = q.PopFrame();
    auto f2 = q.PopFrame();
    auto f3 = q.PopFrame();

    assert(f1.has_value() && *f1 == 102);
    assert(f2.has_value() && *f2 == 103);
    assert(!f3.has_value());

    cout << "ex07 passed!" << endl;
    return 0;
}
