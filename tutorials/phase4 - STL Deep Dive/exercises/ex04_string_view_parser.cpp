/**
 * 練習 4: string_view Parser
 *
 * 解析 "key=value" 格式，回傳 key/value 的 string_view。
 *
 * 重點練習：
 *   - string_view 不擁有資料
 *   - substr 不複製字元
 *   - 回傳的 view 不能比原始字串活得久
 */

#include <cassert>
#include <iostream>
#include <optional>
#include <string>
#include <string_view>
using namespace std;

struct KeyValue {
    string_view key;
    string_view value;
};

optional<KeyValue> parse_key_value(string_view line) {
    // TODO: 找到 '='，切成 key 和 value。
    // 如果沒有 '='，回傳 nullopt。
    size_t pos = line.find('=');
    if (pos == string_view::npos) {
        return nullopt;
    }
    return KeyValue{line.substr(0, pos), line.substr(pos + 1)};
}

int main() {
    string line = "host=localhost";
    optional<KeyValue> parsed = parse_key_value(line);

    assert(parsed.has_value());
    assert(parsed->key == "host");
    assert(parsed->value == "localhost");

    assert(!parse_key_value("invalid").has_value());

    cout << "ex04 passed!" << endl;
    return 0;
}
