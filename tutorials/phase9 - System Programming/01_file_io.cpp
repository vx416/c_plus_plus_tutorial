/**
 * Phase 9-1: File I/O
 *
 * 目錄:
 *   1. 寫入文字檔
 *   2. 讀回文字檔
 *   3. binary mode 的定位
 */

#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
using namespace std;

string read_all_text(const string& path) {
    ifstream in(path);
    if (!in) {
        throw runtime_error("open input failed: " + path);
    }

    string content;
    string line;
    while (getline(in, line)) {
        content += line;
        content += '\n';
    }
    return content;
}

void file_io_demo() {
    // Output:
    // === file I/O ===
    // hello
    // system programming
    cout << "=== file I/O ===" << endl;

    const string path = "build/file_io_demo.txt";
    {
        ofstream out(path);
        out << "hello\n";
        out << "system programming\n";
    }

    cout << read_all_text(path);
    cout << endl;
}

int main() {
    file_io_demo();
    return 0;
}
