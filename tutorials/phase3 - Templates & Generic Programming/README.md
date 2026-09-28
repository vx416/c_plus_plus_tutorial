# Phase 3 - Templates & Generic Programming

## 目錄結構

```
phase3 - Templates & Generic Programming/
├── 01_function_templates.cpp      # function template / type deduction
├── 02_class_templates.cpp         # class template / specialization
├── 03_variadic_templates.cpp      # parameter pack / fold expression
├── 04_sfinae_if_constexpr.cpp     # SFINAE / type traits / if constexpr
├── 05_concepts.cpp                # C++20 concepts / requires
├── 06_macro_vs_template.cpp       # macro token-level vs template type-level
├── exercises/
│   ├── ex01_minmax_template.cpp
│   ├── ex02_stack_template.cpp
│   ├── ex03_tuple_printer.cpp
│   ├── ex04_type_traits.cpp
│   └── ex05_concepts_numeric.cpp
├── Makefile
└── README.md
```

## 使用方式

```bash
cd "tutorials/phase3 - Templates & Generic Programming"

# 編譯全部
make

# 編譯並執行某個範例或練習題
make run TARGET=01_function_templates
make run TARGET=ex01_minmax_template

# 執行所有範例
make run-examples

# 執行所有練習題
make run-exercises

# 清除編譯產物
make clean
```

## 注意

Phase 3 前半段只需要 C++17，但 `05_concepts.cpp` 和 `ex05_concepts_numeric.cpp`
使用 C++20 concepts，所以 Makefile 統一使用 `-std=c++20`。

## 核心觀念

Template 可以先理解成「編譯期的程式碼樣板」。編譯器看到 template 定義時，
不會馬上替所有型別產生版本；通常是等到你真的呼叫或使用某組型別時，
才 instantiate 出那個版本並檢查 function body 是否對該型別合法。

例如：

```cpp
template <typename A, typename B>
auto add(A a, B b) {
    return a + b;
}
```

這不代表任何 `A` 和 `B` 都可以相加，而是代表：

```text
如果某組 A/B 的 a + b 合法，add<A, B> 就能被產生。
如果沒有 operator+，使用那組型別呼叫時才會編譯失敗。
```

Class template 和 template member function 的差別：

```cpp
template <typename T>
class Box {};
```

`T` 屬於整個 class，`Box<int>` 和 `Box<string>` 是不同型別。

```cpp
class Printer {
public:
    template <typename T>
    void print(T value);
};
```

`T` 只屬於 `print()` 這次呼叫，`Printer` 本身不是 template。

`T value` 搭配 `std::move(value)` 是常見的「存一份資料」寫法：

```cpp
explicit Box(T value) : value_(std::move(value)) {}
```

如果呼叫端傳 lvalue，會先 copy 成參數 `value`，再 move 這個區域參數到 member。
所以被 move 的不是呼叫端原本的物件，而是 constructor 裡的那份參數。

## 常見專有名詞

| 名詞 | 白話意思 | 看 code 時要抓的重點 |
|------|----------|----------------------|
| function template | 函式樣板 | 用呼叫參數推導 `T`，產生對應版本 |
| class template | class 樣板 | `Box<int>` 和 `Box<string>` 是不同型別 |
| template member function | method 自己是 template | class 的 `T` 固定，但 method 的 `U` 每次呼叫可不同 |
| non-type template parameter | template 參數是值，不是型別 | `array<T, N>` 的 `N` 是編譯期常數 |
| parameter pack | 一包參數 | `Args...` 是一包型別，`args...` 是一包值 |
| fold expression | 用 operator 展開一包參數 | `(args + ...)` 近似把所有參數串起來相加 |
| type traits | 編譯期詢問型別特性 | `is_integral_v<T>` 這種結果是編譯期 bool |
| SFINAE | 型別代入失敗時，移除候選版本 | 舊式 template 限制技巧，新 code 優先用 concepts |
| concept | template 的型別條件 | `template <integral T>` 表示只接受整數類型 |
| requires expression | 檢查某段 expression 是否合法 | `requires(T a, T b) { a + b; }` 檢查能不能相加 |

## Full vs Partial Specialization

Specialization 是「針對某些型別，改用不同 template 定義」。

Primary template 是預設版本：

```cpp
template <typename T>
struct TypeName {
    static string name() { return "unknown"; }
};
```

Full specialization 是指定一個完整型別，整份改寫：

```cpp
template <>
struct TypeName<int> {
    static string name() { return "int"; }
};
```

這代表只有 `TypeName<int>` 走這份。`TypeName<double>` 不會走。

Partial specialization 是指定一種型別形狀，符合那個形狀的一群型別都走這份：

```cpp
template <typename T>
struct IsPointer<T*> {
    static constexpr bool value = true;
};
```

這代表 `int*`、`double*`、`string*` 都符合 `T*` 這個形狀。

簡單記：

```text
Full specialization:
  我只特別處理 TypeName<int> 這一個完整型別。

Partial specialization:
  我特別處理所有長得像 T* 的型別。
```

## Template vs Macro

Template / generic programming 和 macro 都能減少重複 code，但層級不同：

| 工具 | 層級 | 適合處理 | 不適合處理 |
|------|------|----------|------------|
| Template | type-level / compiler 理解的泛型 | 同一套演算法套不同型別、container、numeric、callable、type traits | 自動取得欄位名稱、拼 C++ token、條件編譯 |
| Macro | token-level / preprocessor 展開 | `#include` guard、`__FILE__` / `__LINE__`、欄位 token 轉字串、重複宣告樣板、簡單 codegen | 複雜邏輯、型別安全、可維護的大型抽象 |

簡單判斷：

```cpp
// 同一套邏輯，不同型別：優先用 template
template <typename T>
const T& max_value(const T& a, const T& b);

// 需要 token 名稱本身：macro 才做得到
#define JSON_FIELD(obj, j, field) j[#field] = obj.field
```

Macro 比較像「編譯前的 token-level code generation」。它很彈，但不懂型別、不懂 scope，錯誤訊息也通常比較難讀。只要 template 做得到，先用 template；template 做不到的語法生成或 metadata，再考慮 macro 或正式 codegen。Macro 會在 Phase 7 的 preprocessor 主題裡深入整理。

## 練習題說明

| 練習 | 對應章節 | 題目 | 難度 |
|------|---------|------|------|
| ex01 | 01 Function Templates | 實作泛型 min/max 與 clamp | 簡單 |
| ex02 | 02 Class Templates | 實作固定容量 Stack<T> | 中等 |
| ex03 | 03 Variadic Templates | 印出任意數量參數 | 中等 |
| ex04 | 04 SFINAE / if constexpr | 用 type traits 做型別分派 | 中等 |
| ex05 | 05 Concepts | 限制 template 只接受數值型別 | 中等 |

## 建議學習順序

先把 01 和 02 看懂，再看 03。  
04 的 SFINAE 是舊式技巧，主要是為了看懂舊 code；新 code 優先使用 05 concepts。
