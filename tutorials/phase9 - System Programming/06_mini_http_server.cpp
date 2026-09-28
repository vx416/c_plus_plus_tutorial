/**
 * Phase 9-6: Mini HTTP Server Shape
 *
 * 真正 HTTP server 會從 socket read request bytes，再 write response bytes。
 * 這裡先把 parser/response 拆出來，避免範例執行時卡在 listen/accept。
 */

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

string build_response(string_view body) {
    return "HTTP/1.1 200 OK\r\nContent-Length: " + to_string(body.size()) +
           "\r\nContent-Type: text/plain\r\n\r\n" + string(body);
}

int main() {
    // Output:
    // === mini HTTP shape ===
    //   method = GET
    //   path = /hello
    //   response:
    // HTTP/1.1 200 OK
    // Content-Length: 5
    // Content-Type: text/plain
    //
    // hello
    cout << "=== mini HTTP shape ===" << endl;

    RequestLine line = parse_request_line("GET /hello HTTP/1.1\r\nHost: localhost\r\n\r\n");
    cout << "  method = " << line.method << endl;
    cout << "  path = " << line.path << endl;
    cout << "  response:\n" << build_response("hello") << endl;
    cout << endl;
    return 0;
}
