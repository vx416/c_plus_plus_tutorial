/**
 * Phase 7-4: Namespace 機制
 *
 * namespace 用來避免名字衝突，並表達 API 所屬範圍。
 *
 * 目錄:
 *   1. basic namespace
 *   2. using 的風險
 *   3. nested namespace
 *   4. ADL
 *   5. anonymous namespace
 */

#include <iostream>
#include <string>
using namespace std;

namespace app {
string name() {
    return "tutorial";
}
}

// ============================================================
// 1. basic namespace
// ============================================================
// 本章重點：
//   namespace 把名字放進範圍，避免和其他 library 撞名。
void basic_namespace_demo() {
    // Output:
    // === basic namespace ===
    //   app::name() = tutorial
    //
    cout << "=== basic namespace ===" << endl;
    cout << "  app::name() = " << app::name() << endl << endl;
}

// ============================================================
// 2. using 的風險
// ============================================================
// 本章重點：
//   using namespace 放在 header 很危險，會污染 include 你 header 的所有檔案。
//   .cpp 裡也要節制；大型專案常偏好明寫 std::vector、app::User。
void using_demo() {
    // Output:
    // === using risk ===
    //   never put using namespace in public headers
    //
    cout << "=== using risk ===" << endl;
    cout << "  never put using namespace in public headers" << endl << endl;
}

// ============================================================
// 3. nested namespace
// ============================================================
// 本章重點：
//   C++17 可寫 namespace company::project。
//   適合組織大型專案 API。
namespace company::project {
int version() {
    return 1;
}
}

void nested_namespace_demo() {
    // Output:
    // === nested namespace ===
    //   version = 1
    //
    cout << "=== nested namespace ===" << endl;
    cout << "  version = " << company::project::version() << endl << endl;
}

// ============================================================
// 4. ADL
// ============================================================
// 本章重點：
//   ADL 是 argument-dependent lookup。
//   呼叫未加 namespace 的函式時，compiler 會根據參數型別所在 namespace 找候選函式。
namespace math {
struct Point {
    int x;
    int y;
};

ostream& operator<<(ostream& os, const Point& point) {
    return os << "(" << point.x << "," << point.y << ")";
}
}

void adl_demo() {
    // Output:
    // === ADL ===
    //   point = (1,2)
    //
    cout << "=== ADL ===" << endl;
    math::Point point{1, 2};
    cout << "  point = " << point << endl << endl;
}

// ============================================================
// 5. anonymous namespace
// ============================================================
// 本章重點：
//   anonymous namespace 讓名字只在目前 translation unit 可見。
//   適合 .cpp 內部 helper，避免外部 link 到它。
namespace {
int internal_helper() {
    return 42;
}
}

void anonymous_namespace_demo() {
    // Output:
    // === anonymous namespace ===
    //   internal helper = 42
    cout << "=== anonymous namespace ===" << endl;
    cout << "  internal helper = " << internal_helper() << endl << endl;
}

int main() {
    basic_namespace_demo();
    using_demo();
    nested_namespace_demo();
    adl_demo();
    anonymous_namespace_demo();
    return 0;
}
