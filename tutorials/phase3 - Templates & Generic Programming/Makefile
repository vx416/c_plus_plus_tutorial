CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra

EXAMPLES = 01_function_templates 02_class_templates 03_variadic_templates \
           04_sfinae_if_constexpr 05_concepts 06_macro_vs_template

EXERCISES = ex01_minmax_template ex02_stack_template ex03_tuple_printer \
            ex04_type_traits ex05_concepts_numeric

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
	@echo "  make                    編譯所有範例和練習題"
	@echo "  make examples           只編譯範例"
	@echo "  make exercises          只編譯練習題"
	@echo "  make run TARGET=01_function_templates"
	@echo "  make run-examples       執行所有範例"
	@echo "  make run-exercises      執行所有練習題"
	@echo "  make clean              清除編譯產物"
