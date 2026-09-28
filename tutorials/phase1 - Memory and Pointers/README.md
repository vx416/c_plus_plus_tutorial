# Phase 1 - Memory and Pointers

## 目錄結構

```
phase1 - Memory and Pointers/
├── 01_pointers.cpp              # 指標基礎
├── 02_references_vs_pointers.cpp # Reference vs Pointer
├── 03_dynamic_memory.cpp        # new / delete
├── 04_ownership_raii.cpp        # Ownership & RAII
├── 05_smart_pointers.cpp        # unique_ptr / shared_ptr / weak_ptr
├── 06_arrays_and_containers.cpp # array / vector / iterator
├── exercises/                   # 練習題（有 assert 測試）
│   ├── ex01_pointer_swap.cpp
│   ├── ex02_array_reverse.cpp
│   ├── ex03_find_max.cpp
│   ├── ex04_dynamic_matrix.cpp
│   ├── ex05_raii_logger.cpp
│   ├── ex06_unique_ptr_list.cpp
│   └── ex07_vector_stats.cpp
├── Makefile
└── README.md
```

## 使用方式

### 用 Makefile（推薦）

```bash
cd "tutorials/phase1 - Memory and Pointers"

# 編譯全部
make

# 編譯並執行某個範例或練習題
make run TARGET=01_pointers
make run TARGET=ex01_pointer_swap

# 執行所有範例
make run-examples

# 執行所有練習題（顯示 PASS/FAIL）
make run-exercises

# 用 AddressSanitizer 編譯（偵測 memory leak）
make sanitize TARGET=ex04_dynamic_matrix

# 清除編譯產物
make clean
```

### 手動編譯

```bash
# 編譯範例
g++ -std=c++17 -Wall -Wextra 01_pointers.cpp -o 01_pointers
./01_pointers

# 編譯練習題
g++ -std=c++17 -Wall -Wextra exercises/ex01_pointer_swap.cpp -o ex01
./ex01

# 用 AddressSanitizer 偵測 memory leak
g++ -std=c++17 -fsanitize=address -g exercises/ex04_dynamic_matrix.cpp -o ex04
./ex04
```

## 練習題說明

每個練習題都有 `TODO` 標記需要實作的部分，完成後執行即可驗證（assert 通過會印出 `passed!`）。

| 練習 | 對應章節 | 題目 | 難度 |
|------|---------|------|------|
| ex01 | 01 Pointers | 用指標實作 swap | 簡單 |
| ex02 | 01 Pointers | 用 pointer arithmetic 反轉陣列 | 簡單 |
| ex03 | 02 References | 回傳最大元素的 reference | 簡單 |
| ex04 | 03 Dynamic Memory | 用 new/delete 實作 2D 矩陣 | 中等 |
| ex05 | 04 RAII | 實作 ScopeLogger | 中等 |
| ex06 | 05 Smart Pointers | 用 unique_ptr 實作 linked list | 中等 |
| ex07 | 06 Containers | vector 統計、過濾、攤平 | 中等 |

## 建議學習順序

按照編號 01 → 06 依序閱讀範例，每個章節讀完後做對應的練習題。
