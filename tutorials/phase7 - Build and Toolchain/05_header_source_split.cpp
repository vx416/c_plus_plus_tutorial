/**
 * Phase 7-5: Header / Source 拆分
 *
 * header 放宣告，source 放定義。
 * 這讓其他 translation unit 知道 API 長相，但不需要看到所有實作。
 *
 * 目錄:
 *   1. declaration vs definition
 *   2. header 放什麼
 *   3. source 放什麼
 *   4. ODR
 *   5. inline / template 例外
 */

#include <iostream>
using namespace std;

// ============================================================
// 1. declaration vs definition
// ============================================================
// 本章重點：
//   declaration 告訴 compiler「有這個東西」。
//   definition 提供真正實作或儲存空間。
int add(int a, int b); // declaration

int add(int a, int b) { // definition
    return a + b;
}

void declaration_definition_demo() {
    // Output:
    // === declaration vs definition ===
    //   add(2, 3) = 5
    //
    cout << "=== declaration vs definition ===" << endl;
    cout << "  add(2, 3) = " << add(2, 3) << endl << endl;
}

// ============================================================
// 2. header 放什麼
// ============================================================
// 本章重點：
//   header 通常放 public declarations：class 宣告、function 宣告、常數、template 定義。
//   header 會被很多 .cpp include，所以要小、穩定、相依少。
void header_demo() {
    // Output:
    // === header content ===
    //   public declarations belong in headers
    //
    cout << "=== header content ===" << endl;
    cout << "  public declarations belong in headers" << endl << endl;
}

// ============================================================
// 3. source 放什麼
// ============================================================
// 本章重點：
//   .cpp 放實作細節。
//   改 .cpp 通常只需要重編該 translation unit；改 header 會讓 include 它的檔案都重編。
void source_demo() {
    // Output:
    // === source content ===
    //   implementation details belong in .cpp files
    //
    cout << "=== source content ===" << endl;
    cout << "  implementation details belong in .cpp files" << endl << endl;
}

// ============================================================
// 4. ODR
// ============================================================
// 本章重點：
//   ODR 是 One Definition Rule。
//   同一個 non-inline function/variable 在整個程式只能有一個定義。
//   把普通 function 定義放 header，可能被多個 .cpp include 後造成 duplicate symbol。
void odr_demo() {
    // Output:
    // === ODR ===
    //   one non-inline definition per program
    //
    cout << "=== ODR ===" << endl;
    cout << "  one non-inline definition per program" << endl << endl;
}

// ============================================================
// 5. inline / template 例外
// ============================================================
// 本章重點：
//   inline function 可以在多個 translation unit 定義，只要內容一致。
//   template 通常要把定義放 header，因為 compiler instantiate 時需要看到實作。
void inline_template_demo() {
    // Output:
    // === inline / template ===
    //   templates usually live in headers
    //   inline definitions must be identical everywhere
    cout << "=== inline / template ===" << endl;
    cout << "  templates usually live in headers" << endl;
    cout << "  inline definitions must be identical everywhere" << endl << endl;
}

int main() {
    declaration_definition_demo();
    header_demo();
    source_demo();
    odr_demo();
    inline_template_demo();
    return 0;
}
