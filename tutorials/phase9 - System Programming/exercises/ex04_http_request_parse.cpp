/**
 * 練習 4: parse HTTP request line
 *
 * 解析 "GET /path HTTP/1.1" 三個欄位。
 */

#include <cassert>
#include <iostream>
#include <sstream>
#include <string>
using namespace std;

struct RequestLine {
    string method;
    string path;
    string version;
};

RequestLine parse_request_line(const string& request) {
    istringstream in(request);
    RequestLine line;
    in >> line.method >> line.path >> line.version;
    return line;
}

int main() {
    RequestLine line = parse_request_line("POST /api/items HTTP/1.1\r\nHost: localhost\r\n\r\n");

    assert(line.method == "POST");
    assert(line.path == "/api/items");
    assert(line.version == "HTTP/1.1");

    cout << "ex04 passed!" << endl;
    return 0;
}
