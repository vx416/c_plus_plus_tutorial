# Phase 5 - Modern C++ Features

## 目錄結構

```
phase5 - Modern C++ Features/
├── 01_move_semantics.cpp                  # rvalue reference / move / forward
├── 02_lambdas.cpp                         # capture / generic lambda / mutable
├── 03_constexpr_consteval.cpp             # compile-time computation
├── 04_structured_bindings_auto_decltype.cpp # structured bindings / auto / decltype
├── 05_error_handling.cpp                  # exception / error code / expected-style result
├── exercises/
│   ├── ex01_move_only_buffer.cpp
│   ├── ex02_lambda_filter.cpp
│   ├── ex03_constexpr_table.cpp
│   ├── ex04_auto_decltype.cpp
│   └── ex05_parse_result.cpp
├── Makefile
└── README.md
```

## 使用方式

```bash
cd "tutorials/phase5 - Modern C++ Features"

make
make run TARGET=01_move_semantics
make run TARGET=ex01_move_only_buffer
make run-examples
make run-exercises
make clean
```

## 注意

Phase 5 使用 `-std=c++23`，因為錯誤處理章節會提到 C++23 的 `std::expected`。
不同 compiler/standard library 對 `std::expected` 的支援時間不一樣，所以範例用
`SimpleExpected` 示範同樣概念，避免整個教學因標準庫缺功能而無法編譯。

## 核心觀念

Modern C++ 的共同方向是：把意圖寫進型別和語法裡。

```text
move semantics     表示資源可以被搬走，不一定要複製
lambda             把小段行為直接寫在使用位置
constexpr          把能在編譯期算的東西提前算掉
auto/decltype      讓型別跟 expression 結果保持一致
structured binding 把 tuple/pair/struct 拆成具名變數
expected-style     回傳成功值或錯誤原因，而不是只給 true/false
```

## 常見專有名詞

| 名詞 | 白話意思 | 看 code 時要抓的重點 |
|------|----------|----------------------|
| lvalue | 有名字、可取位址、通常在語句後還存在 | `x`、`s.name`、回傳 `T&` 的函式呼叫 |
| prvalue | 純粹的值，還沒有對應到任何物件 | `42`、`string("hi")`、`a + b`、回傳 `T` 的函式呼叫 |
| xvalue | 有身分但被標記為可搬走的物件（expiring value） | `std::move(x)`、回傳 `T&&` 的函式呼叫、`std::move(p).first` |
| glvalue | lvalue + xvalue，共同點是「有身分」 | 能取成員、能判斷是哪一個物件 |
| rvalue | xvalue + prvalue，共同點是「可以被搬走」 | 能綁到 `T&&`，overload 會選到 move 版本 |
| rvalue reference | 可以綁定 rvalue 的 reference | `T&&` 在非推導情境是 rvalue reference |
| `std::move` | 把 expression 轉成 rvalue | 它不搬資料，真正搬的是 move ctor/assignment |
| `std::forward` | 保留參數原本 lvalue/rvalue 性質 | forwarding reference 才用 |
| copy elision / RVO（Return Value Optimization） | 回傳 prvalue 時直接在呼叫端建構，不 copy 不 move | C++17 起是語言保證，copy/move 都 delete 的型別也能回傳 |
| NRVO（Named Return Value Optimization） | 回傳具名 local 時省掉 copy/move | 只是允許不是保證；`return std::move(local)` 反而會讓它失效 |
| implicit move | NRVO 做不到時，把回傳的 local/參數當 rvalue 選 constructor | 所以直接 `return b;` 最差也是 move，不會 copy |
| sink parameter | 函式會據為己有的參數（constructor、setter、push_back） | 預設 by-value 再 `std::move`；只讀的參數不是 sink，用 `string_view` / `const T&` |
| lambda capture | lambda 從外部拿變數 | `[=]` copy、`[&]` reference、`[x]` 指定 copy |
| `constexpr` | 可以在編譯期求值 | 不保證每次都在編譯期，取決於使用方式 |
| `consteval` | 一定要在編譯期執行 | runtime 呼叫會編譯失敗 |
| structured binding | 解構 pair/tuple/struct | `auto [key, value] = item;` |
| `decltype` | 取得 expression 的型別 | 保留 reference/const 規則比 `auto` 精細 |
| `invoke_result_t` | 推導 callable 被呼叫後的回傳型別 | 常用在 generic wrapper / thread pool submit |

## 練習題說明

| 練習 | 對應章節 | 題目 | 難度 |
|------|---------|------|------|
| ex01 | 01 Move Semantics | 實作 move-only Buffer | 困難 |
| ex02 | 02 Lambdas | 用 lambda filter / transform | 簡單 |
| ex03 | 03 constexpr | 編譯期產生平方表 | 中等 |
| ex04 | 04 auto/decltype | 保留 reference 的 forwarding helper | 中等 |
| ex05 | 05 Error Handling | parse int 回傳 result | 中等 |

## 建議學習順序

先讀 `01_move_semantics.cpp`，因為 STL、template、lambda capture 都會碰到 move。
接著讀 lambda 和 constexpr。`auto/decltype` 可以在 template 看多後回來反覆讀。
