CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -pthread
SANITIZE_FLAGS = -fsanitize=address,undefined -fno-omit-frame-pointer -g

EXAMPLES = 01_perfetto_tracing_and_counters \
           02_pmu_perf_counters_and_cache \
           03_kernel_driver_debugging \
           04_compiler_optimization_and_ub_traps \
           05_binary_symbols_tombstone_and_hwasan

EXERCISES = ex01_perfetto_trace_builder \
            ex02_false_sharing_and_cache_line \
            ex03_mmio_and_flight_recorder \
            ex04_strict_aliasing_and_bit_cast \
            ex05_tombstone_symbolizer

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

sanitize: $(BUILD_DIR)/$(TARGET)_san
	./$(BUILD_DIR)/$(TARGET)_san

$(BUILD_DIR)/%_san: %.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(SANITIZE_FLAGS) $< -o $@

$(BUILD_DIR)/ex%_san: exercises/ex%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(SANITIZE_FLAGS) $< -o $@

clean:
	rm -rf $(BUILD_DIR)

help:
	@echo "用法:"
	@echo "  make"
	@echo "  make run TARGET=01_perfetto_tracing_and_counters"
	@echo "  make run-examples"
	@echo "  make run-exercises"
	@echo "  make sanitize TARGET=04_compiler_optimization_and_ub_traps"
	@echo "  make clean"
