CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra

EXAMPLES = 01_lexer 02_parser_ast 03_evaluator 04_symbol_table \
           05_bytecode_vm 06_codegen_shape 07_mini_language

EXERCISES = ex01_tokenize_numbers ex02_parse_expression ex03_eval_ast \
            ex04_bytecode_vm ex05_symbol_scope

BUILD_DIR = build

.PHONY: all examples exercises clean help run run-examples run-exercises

all: examples exercises

examples: $(addprefix $(BUILD_DIR)/, $(EXAMPLES))

exercises: $(addprefix $(BUILD_DIR)/, $(EXERCISES))

$(BUILD_DIR)/%: %.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $< -o $@

$(BUILD_DIR)/ex%: exercises/ex%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

run: $(BUILD_DIR)/$(TARGET)
	./$(BUILD_DIR)/$(TARGET)

run-examples: examples
	@for f in $(EXAMPLES); do \
		echo "\n========== $$f =========="; \
		./$(BUILD_DIR)/$$f; \
	done

run-exercises: exercises
	@for f in $(EXERCISES); do \
		echo "--- $$f ---"; \
		./$(BUILD_DIR)/$$f && echo "  PASS" || echo "  FAIL"; \
	done

clean:
	rm -rf $(BUILD_DIR)

help:
	@echo "用法:"
	@echo "  make"
	@echo "  make run TARGET=01_lexer"
	@echo "  make run-examples"
	@echo "  make run-exercises"
	@echo "  make clean"
