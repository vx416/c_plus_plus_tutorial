/**
 * Phase 12-2: Intrusive Reference Counting (android::RefBase / sp<T>) & Custom Deleters
 *
 * 在 Android Native Framework (`libutils` / `libbinder` / Legacy HIDL) 中，
 * 你會看到大量 `android::sp<T>` (Strong Pointer) 與 `android::wp<T>` (Weak Pointer)，
 * 而不是 `std::shared_ptr<T>`。
 *
 * 為什麼 Android 當初設計了侵入式 (Intrusive) 的 `RefBase` + `sp<T>`？
 *   1. `std::shared_ptr<T>` 的引用計數放在外部的 Control Block（若不用 make_shared 會有兩次配置）；
 *      而 `RefBase` 把引用計數直接嵌在物件本體裡面（Intrusive），所以 `sizeof(sp<T>) == sizeof(T*)`（只有 8 bytes，而 shared_ptr 是 16 bytes）。
 *   2. 因為計數在物件內部，把裸指標 `this` 轉回 `sp<T>(this)` 是安全的（`shared_ptr` 則必須繼承 `enable_shared_from_this`）。
 *   3. `RefBase` 提供獨特的 `onFirstRef()` 虛擬函式：當物件第一次被放進 `sp<T>` 時自動觸發！
 *
 * 此外，包裝 C API 或硬體 Handle 時，最乾淨的做法是 `std::unique_ptr<T, CustomDeleter>`。
 *
 * 目錄:
 *   1. 手寫簡化版 android::RefBase 與 sp<T>（理解 intrusive refcount 與 onFirstRef）
 *   2. sp<T> vs std::shared_ptr<T> 對照
 *   3. std::unique_ptr Custom Deleter 包裝硬體 / C 資源
 */

#include <atomic>
#include <iostream>
#include <memory>
#include <string>
using namespace std;

// ============================================================
// 1. 簡化版 android::RefBase 與 sp<T>
// ============================================================
class LightRefBase {
public:
    LightRefBase() = default;
    virtual ~LightRefBase() = default;

    void incStrong() const {
        int32_t old = count_.fetch_add(1, memory_order_relaxed);
        if (old == 0) {
            // Android RefBase 的經典特性：第一次被 sp<T> 接管時呼叫 onFirstRef()
            const_cast<LightRefBase*>(this)->onFirstRef();
        }
    }

    void decStrong() const {
        if (count_.fetch_sub(1, memory_order_acq_rel) == 1) {
            delete this;
        }
    }

    int32_t getStrongCount() const {
        return count_.load(memory_order_relaxed);
    }

protected:
    virtual void onFirstRef() {}

private:
    mutable atomic<int32_t> count_{0};
};

template <typename T>
class sp {
public:
    sp() = default;

    // 從裸指標建構（直接遞增物件內部的計數器）
    sp(T* ptr) : ptr_(ptr) {
        if (ptr_) ptr_->incStrong();
    }

    sp(const sp& other) : ptr_(other.ptr_) {
        if (ptr_) ptr_->incStrong();
    }

    sp(sp&& other) noexcept : ptr_(other.ptr_) {
        other.ptr_ = nullptr;
    }

    ~sp() {
        if (ptr_) ptr_->decStrong();
    }

    sp& operator=(const sp& other) {
        if (this != &other) {
            if (other.ptr_) other.ptr_->incStrong();
            if (ptr_) ptr_->decStrong();
            ptr_ = other.ptr_;
        }
        return *this;
    }

    T* get() const { return ptr_; }
    T* operator->() const { return ptr_; }
    T& operator*() const { return *ptr_; }
    explicit operator bool() const { return ptr_ != nullptr; }

private:
    T* ptr_ = nullptr;
};

class BinderServiceStub : public LightRefBase {
public:
    explicit BinderServiceStub(string name) : name_(std::move(name)) {
        cout << "  [ctor] " << name_ << " (strong_count=0)" << endl;
    }

    ~BinderServiceStub() override {
        cout << "  [dtor] " << name_ << endl;
    }

    const string& name() const { return name_; }

protected:
    void onFirstRef() override {
        cout << "  [onFirstRef] " << name_ << " registered to Binder loop" << endl;
    }

private:
    string name_;
};

void intrusive_sp_demo() {
    // Output:
    // === Intrusive RefBase & sp<T> ===
    //   sizeof(sp<BinderServiceStub>) = 8, sizeof(shared_ptr) = 16
    //   [ctor] CameraService (strong_count=0)
    //   [onFirstRef] CameraService registered to Binder loop
    //   strong count after copy = 2
    //   strong count after inner scope = 1
    //   [dtor] CameraService
    //
    cout << "=== Intrusive RefBase & sp<T> ===" << endl;
    cout << "  sizeof(sp<BinderServiceStub>) = " << sizeof(sp<BinderServiceStub>)
         << ", sizeof(shared_ptr) = " << sizeof(shared_ptr<BinderServiceStub>) << endl;

    {
        sp<BinderServiceStub> s1(new BinderServiceStub("CameraService"));
        {
            sp<BinderServiceStub> s2 = s1;
            cout << "  strong count after copy = " << s1->getStrongCount() << endl;
        }
        cout << "  strong count after inner scope = " << s1->getStrongCount() << endl;
    }
    cout << endl;
}

// ============================================================
// 2. unique_ptr Custom Deleter 包裝 C / 硬體 Handle
// ============================================================
// 本章重點：
//   許多底層 C library（如 V4L2 buffer、dlopen handle、FILE*、AHardwareBuffer*）
//   不是用 `delete` 釋放，而是呼叫專屬的 `close_xxx_handle()`。
//   透過自訂 Functor Deleter，可以零額外開銷把 C handle 變成 RAII unique_ptr！
struct HwIspChannel {
    int channel_id;
};

HwIspChannel* open_isp_channel(int id) {
    cout << "  [C-API] open_isp_channel(" << id << ")" << endl;
    return new HwIspChannel{id};
}

void close_isp_channel(HwIspChannel* ch) {
    if (ch) {
        cout << "  [C-API] close_isp_channel(" << ch->channel_id << ")" << endl;
        delete ch;
    }
}

struct IspChannelDeleter {
    void operator()(HwIspChannel* ch) const {
        close_isp_channel(ch);
    }
};

using ScopedIspChannel = unique_ptr<HwIspChannel, IspChannelDeleter>;

void custom_deleter_demo() {
    // Output:
    // === unique_ptr Custom Deleter ===
    //   sizeof(ScopedIspChannel) = 8
    //   [C-API] open_isp_channel(3)
    //   using channel 3
    //   [C-API] close_isp_channel(3)
    //
    cout << "=== unique_ptr Custom Deleter ===" << endl;
    cout << "  sizeof(ScopedIspChannel) = " << sizeof(ScopedIspChannel) << endl;

    {
        ScopedIspChannel ch(open_isp_channel(3));
        cout << "  using channel " << ch->channel_id << endl;
    }
    cout << endl;
}

int main() {
    intrusive_sp_demo();
    custom_deleter_demo();
    return 0;
}
