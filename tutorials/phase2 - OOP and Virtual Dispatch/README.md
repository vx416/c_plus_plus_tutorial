# Phase 2 - OOP and Virtual Dispatch

## 目錄結構

```
phase2 - OOP and Virtual Dispatch/
├── 01_class_and_struct.cpp       # Class / Struct / access / const / mutable / static
├── 02_rule_of_five.cpp           # Copy / Move / Rule of 3/5/0
├── 03_inheritance.cpp            # 繼承、virtual destructor、slicing
├── 04_polymorphism.cpp           # virtual / override / vtable / 多型容器
├── 05_operator_overloading.cpp   # + - == << [] ++ / friend
├── 06_rtti_and_cast.cpp          # typeid / static_cast / dynamic_cast
├── exercises/
│   ├── ex01_point.cpp
│   ├── ex02_string_buffer.cpp
│   ├── ex03_shape_hierarchy.cpp
│   ├── ex04_complex_number.cpp
│   └── ex05_observer_pattern.cpp
├── Makefile
└── README.md
```

## 使用方式

```bash
cd "tutorials/phase2 - OOP and Virtual Dispatch"

# 編譯全部
make

# 執行某個範例或練習題
make run TARGET=01_class_and_struct
make run TARGET=ex01_point

# 執行所有練習題（顯示 PASS/FAIL/SKIP）
make run-exercises

# 用 AddressSanitizer（偵測 memory leak，適合 ex02）
make sanitize TARGET=ex02_string_buffer
```

## 練習題說明

| 練習 | 對應章節 | 題目 | 難度 |
|------|---------|------|------|
| ex01 | 01 Class | 實作 2D Point（constructor / const method） | 簡單 |
| ex02 | 02 Rule of Five | 實作 StringBuffer（深拷貝、move） | 困難 |
| ex03 | 03+04 繼承/多型 | Shape 階層（Circle/Rectangle/Triangle） | 中等 |
| ex04 | 05 Operator | 複數 Complex（+ - * == <<） | 中等 |
| ex05 | 03+04 繼承/多型 | Observer Pattern（interface + 多型） | 中等 |

## 章節重點速查

### 01 Class & Struct
- struct 預設 public、class 預設 private
- Constructor 初始化列表比 body 賦值快
- `const` 方法承諾不修改成員
- `mutable` 只給 cache、mutex、metrics 這類內部細節使用，不應拿來偷改真正資料
- `static` 成員屬於 class，不屬於物件

### 02 Rule of Three/Five/Zero
- 有 raw 資源 → 完整實作 Rule of Five
- 用 STL container / smart pointer → 適用 Rule of Zero
- `move` 要標 `noexcept` 才會被 STL 容器使用

### 03 Inheritance
- **Base class 的 destructor 一律 virtual**
- 建構順序：base → derived；解構順序反過來
- 多型物件絕對不要 by value 傳遞（slicing）

### 04 Polymorphism
- `virtual` 讓函式呼叫變成執行時綁定
- `override` 讓編譯器幫你檢查簽名
- 多型容器用 `vector<unique_ptr<Base>>`

### 05 Operator Overloading
- 非對稱運算子用成員函式，對稱用非成員 + `friend`
- `<<` 永遠是非成員函式
- 前置 `++` 比後置有效率（不產生副本）

### 06 RTTI & Cast
- `static_cast`：編譯時、已知安全
- `dynamic_cast`：執行時、多型向下轉型
- 大量 `dynamic_cast` 通常是設計味道，考慮改用 virtual

## 建議學習順序

按編號 01 → 06 依序閱讀。02 (Rule of Five) 和 06 (RTTI) 是比較深的主題，可以先看過概念再回頭深入。
