/**
 * 練習 1: copy file
 *
 * 用 fstream 實作文字檔複製。
 */

#include <cassert>
#include <fstream>
#include <iostream>
#include <string>
using namespace std;

void copy_file(const string& from, const string& to) {
    ifstream in(from);
    ofstream out(to);

    string line;
    while (getline(in, line)) {
        out << line << '\n';
    }
}

string read_all(const string& path) {
    ifstream in(path);
    string content;
    string line;
    while (getline(in, line)) {
        content += line;
        content += '\n';
    }
    return content;
}

int main() {
    const string src = "build/ex01_src.txt";
    const string dst = "build/ex01_dst.txt";

    {
        ofstream out(src);
        out << "alpha\nbeta\n";
    }

    copy_file(src, dst);
    assert(read_all(dst) == "alpha\nbeta\n");

    cout << "ex01 passed!" << endl;
    return 0;
}
