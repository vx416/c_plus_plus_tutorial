/**
 * 練習 2: 實作侵入式智慧指標 LightRefBase 與 sp<T>
 *
 * 請完成 LightRefBase::incStrong / decStrong 以及 sp<T> 的 copy/move/reset，
 * 使得：
 *   - 第一次被 sp<T> 持有時自動觸發 onFirstRef()
 *   - 當最後一個 sp<T> 釋放時自動 delete 物件並觸發 destructor
 */

#include <atomic>
#include <cassert>
#include <iostream>
using namespace std;

class LightRefBase {
public:
    LightRefBase() = default;
    virtual ~LightRefBase() = default;

    void incStrong() const {
        int32_t prev = count_.fetch_add(1, memory_order_relaxed);
        if (prev == 0) {
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
        reset();
    }

    sp& operator=(const sp& other) {
        if (this != &other) {
            if (other.ptr_) other.ptr_->incStrong();
            if (ptr_) ptr_->decStrong();
            ptr_ = other.ptr_;
        }
        return *this;
    }

    sp& operator=(sp&& other) noexcept {
        if (this != &other) {
            if (ptr_) ptr_->decStrong();
            ptr_ = other.ptr_;
            other.ptr_ = nullptr;
        }
        return *this;
    }

    void reset(T* new_ptr = nullptr) {
        if (new_ptr) new_ptr->incStrong();
        if (ptr_) ptr_->decStrong();
        ptr_ = new_ptr;
    }

    T* get() const { return ptr_; }
    T* operator->() const { return ptr_; }
    T& operator*() const { return *ptr_; }

private:
    T* ptr_ = nullptr;
};

struct HalBufferHandle : public LightRefBase {
    int* first_ref_counter;
    int* dtor_counter;

    HalBufferHandle(int* first_ref, int* dtor)
        : first_ref_counter(first_ref), dtor_counter(dtor) {}

    ~HalBufferHandle() override {
        ++(*dtor_counter);
    }

protected:
    void onFirstRef() override {
        ++(*first_ref_counter);
    }
};

int main() {
    int first_ref_calls = 0;
    int dtor_calls = 0;

    {
        sp<HalBufferHandle> h1(new HalBufferHandle(&first_ref_calls, &dtor_calls));
        assert(first_ref_calls == 1);
        assert(dtor_calls == 0);
        assert(h1->getStrongCount() == 1);

        {
            sp<HalBufferHandle> h2 = h1;
            assert(first_ref_calls == 1);  // onFirstRef 只在 0 -> 1 時觸發一次
            assert(h1->getStrongCount() == 2);

            sp<HalBufferHandle> h3 = std::move(h2);
            assert(h2.get() == nullptr);
            assert(h3->getStrongCount() == 2);
        }

        assert(h1->getStrongCount() == 1);
        assert(dtor_calls == 0);
    }

    assert(dtor_calls == 1);

    cout << "ex02 passed!" << endl;
    return 0;
}
