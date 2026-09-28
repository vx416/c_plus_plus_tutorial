CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra
SANITIZE_FLAGS = -fsanitize=address,undefined -fno-omit-frame-pointer

EXAMPLES = 01_compilation_model 02_preprocessor_macro 03_include_mechanism \
           04_namespace 05_header_source_split 06_library_types \
           07_package_management 08_cmake_basics 09_third_party_management \
           10_sanitizers_debugging 11_unit_testing 12_dynamic_loading

EXERCISES = ex01_include_guard ex02_macro_debug ex03_namespace_api \
            ex04_static_library_shape ex05_assert_tests

BUILD_DIR = build

.PHONY: all examples exercises clean help run run-examples run-exercises sanitize

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

sanitize: $(BUILD_DIR)/$(TARGET)
	$(CXX) $(CXXFLAGS) $(SANITIZE_FLAGS) $(TARGET).cpp -o $(BUILD_DIR)/$(TARGET)_sanitize
	./$(BUILD_DIR)/$(TARGET)_sanitize

clean:
	rm -rf $(BUILD_DIR)

help:
	@echo "用法:"
	@echo "  make"
	@echo "  make run TARGET=01_compilation_model"
	@echo "  make run-examples"
	@echo "  make run-exercises"
	@echo "  make sanitize TARGET=10_sanitizers_debugging"
	@echo "  make clean"
