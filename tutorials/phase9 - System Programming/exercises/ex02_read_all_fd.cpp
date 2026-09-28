/**
 * 練習 2: read all by file descriptor
 *
 * 用 open/read/close 讀完整文字檔。
 */

#include <fcntl.h>
#include <unistd.h>

#include <cassert>
#include <fstream>
#include <iostream>
#include <string>
using namespace std;

string read_all_fd(const string& path) {
    int fd = open(path.c_str(), O_RDONLY);
    assert(fd != -1);

    string result;
    char buffer[8];
    while (true) {
        ssize_t n = read(fd, buffer, sizeof(buffer));
        assert(n >= 0);
        if (n == 0) break;
        result.append(buffer, static_cast<size_t>(n));
    }

    close(fd);
    return result;
}

int main() {
    const string path = "build/ex02.txt";
    {
        ofstream out(path);
        out << "hello fd reader";
    }

    assert(read_all_fd(path) == "hello fd reader");

    cout << "ex02 passed!" << endl;
    return 0;
}
