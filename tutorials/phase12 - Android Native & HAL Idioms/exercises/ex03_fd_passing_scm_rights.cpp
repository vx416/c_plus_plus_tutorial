/**
 * 練習 3: 跨行程傳遞 File Descriptor 與 mmap 零拷貝 (模擬 Binder ParcelFileDescriptor)
 *
 * 子行程建立一個 shared memory file 並透過 mmap 寫入 uint32_t magic = 0xCAFEBABE，
 * 接著透過 UNIX Domain Socket (SCM_RIGHTS) 把該 FD 傳給父行程；
 * 父行程收到 FD 後，直接用 mmap 讀取並驗證 magic == 0xCAFEBABE。
 */

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>

#include <cassert>
#include <cstdint>
#include <cstring>
#include <iostream>
using namespace std;

bool send_fd(int sock, int fd_to_send) {
    char dummy = 'B';
    iovec iov{&dummy, sizeof(dummy)};

    alignas(cmsghdr) char cmsg_buf[CMSG_SPACE(sizeof(int))]{};
    msghdr msg{};
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;
    msg.msg_control = cmsg_buf;
    msg.msg_controllen = sizeof(cmsg_buf);

    cmsghdr* cmsg = CMSG_FIRSTHDR(&msg);
    cmsg->cmsg_level = SOL_SOCKET;
    cmsg->cmsg_type = SCM_RIGHTS;
    cmsg->cmsg_len = CMSG_LEN(sizeof(int));
    memcpy(CMSG_DATA(cmsg), &fd_to_send, sizeof(int));

    return sendmsg(sock, &msg, 0) >= 0;
}

int recv_fd(int sock) {
    char dummy = 0;
    iovec iov{&dummy, sizeof(dummy)};

    alignas(cmsghdr) char cmsg_buf[CMSG_SPACE(sizeof(int))]{};
    msghdr msg{};
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;
    msg.msg_control = cmsg_buf;
    msg.msg_controllen = sizeof(cmsg_buf);

    if (recvmsg(sock, &msg, 0) <= 0) {
        return -1;
    }

    cmsghdr* cmsg = CMSG_FIRSTHDR(&msg);
    if (cmsg && cmsg->cmsg_level == SOL_SOCKET && cmsg->cmsg_type == SCM_RIGHTS) {
        int fd = -1;
        memcpy(&fd, CMSG_DATA(cmsg), sizeof(int));
        return fd;
    }
    return -1;
}

int main() {
    int socks[2];
    assert(socketpair(AF_UNIX, SOCK_STREAM, 0, socks) == 0);

    pid_t pid = fork();
    assert(pid >= 0);

    if (pid == 0) {
        close(socks[0]);
        int shm_fd = open("build/ex03_gralloc_buf.bin", O_CREAT | O_TRUNC | O_RDWR, 0600);
        assert(shm_fd >= 0);
        assert(ftruncate(shm_fd, sizeof(uint32_t)) == 0);

        void* ptr = mmap(nullptr, sizeof(uint32_t), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
        assert(ptr != MAP_FAILED);
        *static_cast<uint32_t*>(ptr) = 0xCAFEBABEu;
        munmap(ptr, sizeof(uint32_t));

        assert(send_fd(socks[1], shm_fd));
        close(shm_fd);
        close(socks[1]);
        _exit(0);
    }

    close(socks[1]);
    int received_fd = recv_fd(socks[0]);
    close(socks[0]);

    int status = 0;
    waitpid(pid, &status, 0);
    assert(WIFEXITED(status) && WEXITSTATUS(status) == 0);
    assert(received_fd >= 0);

    void* parent_ptr = mmap(nullptr, sizeof(uint32_t), PROT_READ, MAP_SHARED, received_fd, 0);
    assert(parent_ptr != MAP_FAILED);
    uint32_t magic = *static_cast<const uint32_t*>(parent_ptr);
    assert(magic == 0xCAFEBABEu);

    munmap(parent_ptr, sizeof(uint32_t));
    close(received_fd);

    cout << "ex03 passed!" << endl;
    return 0;
}
