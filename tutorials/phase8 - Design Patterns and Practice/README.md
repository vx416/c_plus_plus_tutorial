# Phase 8 - Design Patterns and Practice

## 目錄結構

```
phase8 - Design Patterns and Practice/
├── 01_solid.cpp                  # SOLID in C++
├── 02_common_patterns.cpp        # Singleton / Factory / Observer / Strategy
├── 03_type_erasure.cpp           # std::function / hand-written type erasure
├── 04_crtp.cpp                   # static polymorphism / mixin
├── 05_mini_project.cpp           # small pipeline project
├── exercises/
│   ├── ex01_strategy_discount.cpp
│   ├── ex02_observer_events.cpp
│   ├── ex03_type_erased_command.cpp
│   ├── ex04_crtp_counter.cpp
│   └── ex05_factory_pipeline.cpp
├── Makefile
└── README.md
```

## 使用方式

```bash
cd "tutorials/phase8 - Design Patterns and Practice"

make
make run TARGET=01_solid
make run-examples
make run-exercises
make clean
```

## 核心觀念

Design pattern 不是要背名字，而是辨認「哪裡會變」。

```text
Strategy     演算法會變
Factory      建立哪個具體型別會變
Observer     事件發生後誰要反應會變
Type erasure 呼叫端不想知道具體型別
CRTP         想要 compile-time polymorphism 或 mixin
```

務實規則：先讓 code 清楚直接。當變動點真的出現，再抽出 pattern。不要為了套 pattern 而加抽象。

## 練習題說明

| 練習 | 對應章節 | 題目 | 難度 |
|------|---------|------|------|
| ex01 | Strategy | 折扣策略 | 簡單 |
| ex02 | Observer | 事件通知 | 中等 |
| ex03 | Type Erasure | type-erased command | 中等 |
| ex04 | CRTP | mixin counter | 中等 |
| ex05 | Factory + Pipeline | 建立並執行處理器 | 中等 |
