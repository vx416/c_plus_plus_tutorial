/**
 * Phase 9-5: Socket Basics
 *
 * socketpair 建立一組已連線的本機 socket。
 * 這適合教學 read/write 模型，不需要真的開 TCP port。
 */

#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <iostream>
#include <stdexcept>
#include <string>
using namespace std;

string socket_echo_once(string_view input) {
    int sockets[2];
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, sockets) == -1) {
        throw runtime_error("socketpair failed");
    }

    write(sockets[0], input.data(), input.size());

    char buffer[64] = {};
    ssize_t n = read(sockets[1], buffer, sizeof(buffer));
    if (n < 0) {
        throw runtime_error("read failed");
    }

    write(sockets[1], buffer, static_cast<size_t>(n));

    char echoed[64] = {};
    n = read(sockets[0], echoed, sizeof(echoed));
    if (n < 0) {
        throw runtime_error("read echo failed");
    }

    close(sockets[0]);
    close(sockets[1]);
    return string(echoed, static_cast<size_t>(n));
}

int main() {
    // Output:
    // === socket basics ===
    //   echo = ping
    cout << "=== socket basics ===" << endl;
    cout << "  echo = " << socket_echo_once("ping") << endl;
    cout << endl;
    return 0;
}
