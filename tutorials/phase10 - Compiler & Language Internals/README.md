# Phase 10 - Compiler & Language Internals

## 目錄結構

```
phase10 - Compiler & Language Internals/
├── 01_lexer.cpp             # tokenization
├── 02_parser_ast.cpp        # recursive descent parser / AST
├── 03_evaluator.cpp         # AST evaluator
├── 04_symbol_table.cpp      # symbol table / scope
├── 05_bytecode_vm.cpp       # stack VM
├── 06_codegen_shape.cpp     # code generation shape
├── 07_mini_language.cpp     # mini language end-to-end
├── exercises/
│   ├── ex01_tokenize_numbers.cpp
│   ├── ex02_parse_expression.cpp
│   ├── ex03_eval_ast.cpp
│   ├── ex04_bytecode_vm.cpp
│   └── ex05_symbol_scope.cpp
├── Makefile
└── README.md
```

## 使用方式

```bash
cd "tutorials/phase10 - Compiler & Language Internals"

make
make run TARGET=01_lexer
make run-examples
make run-exercises
make clean
```

## 核心觀念

Compiler 可以拆成一條 pipeline：

```text
source text
  -> lexer: 字元切成 token
  -> parser: token 組成 AST
  -> semantic analysis: symbol table / type check
  -> code generation: AST 或 IR 轉成 bytecode / assembly / machine code
  -> runtime / VM: 執行產物
```

這個 phase 用一個迷你算術語言示範核心形狀。語言支援整數、`+ - * /`、括號和簡單變數。

## 實務心法

先把資料結構切清楚，比一開始追求 parser 技巧更重要：

```text
Token  描述原始文字的片段
AST    描述語法結構，不再關心空白與原始字元
Scope  描述名稱如何被解析
IR/VM  描述更接近執行的指令
```

## 練習題說明

| 練習 | 對應章節 | 題目 | 難度 |
|------|---------|------|------|
| ex01 | 01 Lexer | tokenize 整數和加號 | 簡單 |
| ex02 | 02 Parser | parse 加乘優先權 | 中等 |
| ex03 | 03 Evaluator | AST 求值 | 中等 |
| ex04 | 05 VM | stack bytecode VM | 中等 |
| ex05 | 04 Scope | symbol table shadowing | 中等 |
