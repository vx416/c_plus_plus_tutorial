CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra

EXAMPLES = 01_solid 02_common_patterns 03_type_erasure 04_crtp 05_mini_project

EXERCISES = ex01_strategy_discount ex02_observer_events ex03_type_erased_command \
            ex04_crtp_counter ex05_factory_pipeline

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
	@echo "  make run TARGET=01_solid"
	@echo "  make run-examples"
	@echo "  make run-exercises"
	@echo "  make clean"
