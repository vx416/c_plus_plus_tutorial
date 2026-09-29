/**
 * 練習 6: 實作 Move-Only RAII UniqueFd
 *
 * 請完成 UniqueFd class：
 *   - 預設建構時 fd_ 為 -1
 *   - 支援 move constructor 與 move assignment
 *   - 禁止 copy constructor 與 copy assignment
 *   - 實作 ok()、get()、release()、reset()
 */

#include <fcntl.h>
#include <unistd.h>

#include <cassert>
#include <cerrno>
#include <iostream>
#include <utility>
using namespace std;

class UniqueFd {
public:
    UniqueFd() = default;
    explicit UniqueFd(int fd) : fd_(fd) {}
    ~UniqueFd() { reset(); }

    UniqueFd(const UniqueFd&) = delete;
    UniqueFd& operator=(const UniqueFd&) = delete;

    UniqueFd(UniqueFd&& other) noexcept : fd_(other.release()) {}

    UniqueFd& operator=(UniqueFd&& other) noexcept {
        if (this != &other) {
            reset(other.release());
        }
        return *this;
    }

    int get() const { return fd_; }
    bool ok() const { return fd_ >= 0; }

    int release() noexcept {
        return std::exchange(fd_, -1);
    }

    void reset(int new_fd = -1) noexcept {
        if (fd_ >= 0) {
            close(fd_);
        }
        fd_ = new_fd;
    }

private:
    int fd_ = -1;
};

int main() {
    UniqueFd empty_fd;
    assert(!empty_fd.ok());
    assert(empty_fd.get() == -1);

    int raw_fd = open("/dev/null", O_RDONLY);
    assert(raw_fd >= 0);

    {
        UniqueFd fd1(raw_fd);
        assert(fd1.ok());
        assert(fd1.get() == raw_fd);

        UniqueFd fd2 = std::move(fd1);
        assert(!fd1.ok());
        assert(fd1.get() == -1);
        assert(fd2.ok());
        assert(fd2.get() == raw_fd);
    }

    // 離開 scope 後 raw_fd 應該已經被自動 close，再次 close 會回傳 -1 (EBADF)
    assert(close(raw_fd) == -1 && errno == EBADF);

    cout << "ex06 passed!" << endl;
    return 0;
}
