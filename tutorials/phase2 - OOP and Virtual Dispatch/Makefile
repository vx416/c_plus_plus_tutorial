CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra
SANITIZE_FLAGS = -fsanitize=address -g

EXAMPLES = 01_class_and_struct 02_rule_of_five 03_inheritance \
           04_polymorphism 05_operator_overloading 06_rtti_and_cast

EXERCISES = ex01_point ex02_string_buffer ex03_shape_hierarchy \
            ex04_complex_number ex05_observer_pattern

BUILD_DIR = build

.PHONY: all examples exercises clean help run run-examples run-exercises sanitize

all: examples exercises

examples: $(addprefix $(BUILD_DIR)/, $(EXAMPLES))

exercises:
	@for f in $(EXERCISES); do \
		$(MAKE) --no-print-directory $(BUILD_DIR)/$$f 2>&1 || echo "  [SKIP] $$f (編譯失敗，可能有 TODO 未完成)"; \
	done

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

run-exercises:
	@for f in $(EXERCISES); do \
		echo "--- $$f ---"; \
		$(MAKE) --no-print-directory $(BUILD_DIR)/$$f 2>/dev/null && \
		(./$(BUILD_DIR)/$$f && echo "  PASS" || echo "  FAIL") || \
		echo "  SKIP (編譯失敗)"; \
	done

sanitize: $(BUILD_DIR)/$(TARGET)_san
$(BUILD_DIR)/%_san: %.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(SANITIZE_FLAGS) $< -o $@
$(BUILD_DIR)/ex%_san: exercises/ex%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(SANITIZE_FLAGS) $< -o $@

clean:
	rm -rf $(BUILD_DIR)

help:
	@echo "用法:"
	@echo "  make                    編譯所有範例和練習題"
	@echo "  make examples           只編譯範例"
	@echo "  make exercises          只編譯練習題"
	@echo "  make run TARGET=01_class_and_struct  編譯並執行指定檔案"
	@echo "  make run-examples       執行所有範例"
	@echo "  make run-exercises      執行所有練習題"
	@echo "  make sanitize TARGET=ex02_string_buffer  用 ASan 編譯"
	@echo "  make clean              清除編譯產物"
