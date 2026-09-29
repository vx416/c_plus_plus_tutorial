# Phase 4 - STL Deep Dive

## 目錄結構

```
phase4 - STL Deep Dive/
├── 01_containers.cpp              # sequence / associative / unordered containers
├── 02_iterators.cpp               # iterator category / invalidation / custom iterator
├── 03_algorithms.cpp              # sort / find / transform / accumulate / ranges
├── 04_string_and_string_view.cpp  # string ownership / string_view lifetime
├── 05_vocabulary_types.cpp        # optional / variant / any
├── 06_span_and_strong_id.cpp      # std::span 連續記憶體視圖 / StrongId 強型別包裝
├── exercises/
│   ├── ex01_container_choice.cpp
│   ├── ex02_iterator_range.cpp
│   ├── ex03_algorithm_pipeline.cpp
│   ├── ex04_string_view_parser.cpp
│   ├── ex05_variant_message.cpp
│   └── ex06_span_and_strong_id.cpp
├── Makefile
└── README.md
```

## 使用方式

```bash
cd "tutorials/phase4 - STL Deep Dive"

make
make run TARGET=01_containers
make run TARGET=ex01_container_choice
make run-examples
make run-exercises
make clean
```

## 核心觀念

STL 可以先理解成三層：

```text
container  負責存資料，例如 vector、list、map、unordered_map
iterator   負責描述「怎麼走訪資料」
algorithm  負責操作一段 iterator 範圍，例如 sort、find、transform
```

這三層分開後，同一個 algorithm 可以套很多 container，只要該 container 的 iterator
能力足夠。例如 `std::sort` 需要 random access iterator，所以可以排序 `vector`，
但不能直接排序 `list`。

## 常見專有名詞

| 名詞 | 白話意思 | 看 code 時要抓的重點 |
|------|----------|----------------------|
| sequence container | 按順序存資料 | `vector` 連續記憶體，`list` 節點分散 |
| associative container | 依 key 排序存資料 | `map` / `set` 通常是 tree，查找 O(log n) |
| unordered container | hash table | `unordered_map` 平均 O(1)，但沒有排序 |
| iterator | 像泛化 pointer 的走訪工具 | algorithm 用 iterator 操作範圍 |
| iterator invalidation | iterator/reference 失效 | container 擴容或刪除元素後要小心 |
| range | 一段可走訪資料 | C++20 ranges 讓 algorithm 更接近直接吃 container |
| `string_view` | 不擁有字串的 view | 不能比原字串活得久 |
| `optional<T>` | 可能有 T，也可能沒有 | 取代「特殊值代表不存在」 |
| `variant<A, B>` | 多選一的型別安全 union | 用 `visit` 處理目前是哪一種 |
| `any` | 可以裝任意型別 | 彈性高，但失去靜態型別資訊 |
| `span<T>` | 不擁有連續記憶體的 view | 可同時接受 `vector` / `array` / raw array，支援 `subspan` 切片 |
| `StrongId` | 強型別整數包裝 | 用空 Tag struct 讓 `SensorId` 與 `RequestId` 在編譯期無法混用 |

## Container 選擇

| 需求 | 優先考慮 |
|------|----------|
| 大多數一般列表、需要 cache locality | `vector` |
| 頻繁在中間插入/刪除，且已經有 iterator | `list` |
| key-value 且需要排序 | `map` |
| key-value 且只在乎快速查找 | `unordered_map` |
| 不重複集合且需要排序 | `set` |
| 不重複集合且只在乎存在與否 | `unordered_set` |

簡單規則：不知道選什麼時，先用 `vector`。等你有明確查找、排序、插入刪除需求，再換成更合適的 container。

## 練習題說明

| 練習 | 對應章節 | 題目 | 難度 |
|------|---------|------|------|
| ex01 | 01 Containers | 統計字串出現次數並輸出排序結果 | 簡單 |
| ex02 | 02 Iterators | 實作簡單整數 range iterator | 中等 |
| ex03 | 03 Algorithms | 用 STL algorithm 做 filter/transform/sum | 中等 |
| ex04 | 04 string_view | 用 `string_view` 切 key/value | 中等 |
| ex05 | 05 vocabulary types | 用 `variant` 表示訊息並處理 | 中等 |
| ex06 | 06 span & StrongId | 用 `std::span` 切封包 payload 並實作 `StrongId` | 中等 |

## 建議學習順序

按 `01 -> 06` 讀。`02_iterators.cpp` 是理解 STL 的關鍵；如果一開始覺得抽象，
先看懂 `begin()` / `end()` / iterator invalidation，再回頭看 custom iterator。
