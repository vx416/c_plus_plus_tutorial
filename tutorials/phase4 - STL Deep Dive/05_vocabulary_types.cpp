/**
 * Phase 4-5: std::optional / std::variant / std::any
 *
 * 這三個常被稱為 vocabulary types。
 * 它們不是容器，而是用標準型別表達常見語意：
 *   optional: 可能沒有值
 *   variant: 幾種型別中選一種
 *   any: 任意型別
 *
 * 目錄:
 *   1. optional: explicit absence
 *   2. variant: type-safe union
 *   3. visit: handle variant
 *   4. any: dynamic typed box
 *   5. 選擇準則
 */

#include <any>
#include <iostream>
#include <optional>
#include <string>
#include <variant>
#include <vector>
using namespace std;

// ============================================================
// 1. optional: explicit absence
// ============================================================
// 本章重點：
//   optional<T> 表示「可能有 T，也可能沒有」。
//   它比回傳 -1、空字串、nullptr 這類特殊值更清楚。
optional<int> find_score(const vector<pair<string, int>>& scores, string_view name) {
    for (const auto& [user, score] : scores) {
        if (user == name) {
            return score;
        }
    }
    return nullopt;
}

void optional_demo() {
    // Output:
    // === optional ===
    //   Alice score = 90
    //   Carol exists = false
    //
    cout << "=== optional ===" << endl;

    vector<pair<string, int>> scores{{"Alice", 90}, {"Bob", 80}};
    optional<int> score = find_score(scores, "Alice");
    if (score.has_value()) {
        cout << "  Alice score = " << *score << endl;
    }

    cout << "  Carol exists = " << boolalpha << find_score(scores, "Carol").has_value()
         << noboolalpha << endl;
    cout << endl;
}

// ============================================================
// 2. variant: type-safe union
// ============================================================
// 本章重點：
//   variant<A, B, C> 表示「目前是 A/B/C 其中一種」。
//   它比 union 安全，因為 variant 知道目前裝的是哪個型別。
struct TextMessage {
    string text;
};

struct ImageMessage {
    string url;
};

using Message = variant<TextMessage, ImageMessage>;

void variant_demo() {
    // Output:
    // === variant ===
    //   holds TextMessage = true
    //
    cout << "=== variant ===" << endl;

    Message message = TextMessage{"hello"};
    cout << "  holds TextMessage = " << boolalpha << holds_alternative<TextMessage>(message)
         << noboolalpha << endl;
    cout << endl;
}

// ============================================================
// 3. visit: handle variant
// ============================================================
// 本章重點：
//   visit 是處理 variant 的標準方式。
//   你提供一個 callable，variant 會根據目前實際型別呼叫正確 overload。
struct MessagePrinter {
    void operator()(const TextMessage& message) const {
        cout << "  text: " << message.text << endl;
    }

    void operator()(const ImageMessage& message) const {
        cout << "  image: " << message.url << endl;
    }
};

void visit_demo() {
    // Output:
    // === visit ===
    //   text: hello
    //   image: https://example.com/cat.png
    //
    cout << "=== visit ===" << endl;

    vector<Message> messages{
        TextMessage{"hello"},
        ImageMessage{"https://example.com/cat.png"},
    };

    for (const Message& message : messages) {
        visit(MessagePrinter{}, message);
    }
    cout << endl;
}

// ============================================================
// 4. any: dynamic typed box
// ============================================================
// 本章重點：
//   any 可以裝任意 copyable 型別。
//   彈性很高，但取出時要知道正確型別；型別錯會丟 bad_any_cast。
//   一般業務邏輯優先考慮 variant，真的需要任意型別時才用 any。
void any_demo() {
    // Output:
    // === any ===
    //   type name = NSt3__112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEEE
    //   value = dynamic text
    //
    cout << "=== any ===" << endl;

    any value = string("dynamic text");
    cout << "  type name = " << value.type().name() << endl;
    cout << "  value = " << any_cast<string>(value) << endl;
    cout << endl;
}

// ============================================================
// 5. 選擇準則
// ============================================================
// 本章重點：
//   optional: 值可能不存在。
//   variant: 型別集合已知，而且只會是其中一種。
//   any: 型別集合未知或插件式擴充，但要接受 runtime cast 成本和風險。
void choice_demo() {
    // Output:
    // === vocabulary type choice ===
    //   missing value: optional<T>
    //   one of known types: variant<A, B>
    //   truly dynamic value: any
    cout << "=== vocabulary type choice ===" << endl;
    cout << "  missing value: optional<T>" << endl;
    cout << "  one of known types: variant<A, B>" << endl;
    cout << "  truly dynamic value: any" << endl;
    cout << endl;
}

int main() {
    optional_demo();
    variant_demo();
    visit_demo();
    any_demo();
    choice_demo();
    return 0;
}
