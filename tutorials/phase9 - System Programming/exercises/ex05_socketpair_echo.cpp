/**
 * 練習 5: socketpair echo
 *
 * 用一組本機 socket 模擬 client/server echo。
 */

#include <sys/socket.h>
#include <unistd.h>

#include <cassert>
#include <iostream>
#include <string>
using namespace std;

string echo_once(string_view message) {
    int sockets[2];
    assert(socketpair(AF_UNIX, SOCK_STREAM, 0, sockets) == 0);

    assert(write(sockets[0], message.data(), message.size()) == static_cast<ssize_t>(message.size()));

    char server_buffer[64] = {};
    ssize_t n = read(sockets[1], server_buffer, sizeof(server_buffer));
    assert(n >= 0);
    assert(write(sockets[1], server_buffer, static_cast<size_t>(n)) == n);

    char client_buffer[64] = {};
    n = read(sockets[0], client_buffer, sizeof(client_buffer));
    assert(n >= 0);

    close(sockets[0]);
    close(sockets[1]);
    return string(client_buffer, static_cast<size_t>(n));
}

int main() {
    assert(echo_once("ping") == "ping");
    assert(echo_once("hello") == "hello");

    cout << "ex05 passed!" << endl;
    return 0;
}
