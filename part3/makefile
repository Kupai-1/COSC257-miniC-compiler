# compiler and flags
CXX = g++
LLVM_CONFIG = llvm-config-18
CXXFLAGS = -g -Wall -std=c++11 $(shell $(LLVM_CONFIG) --cxxflags)
LDFLAGS = $(shell $(LLVM_CONFIG) --ldflags --libs core irreader support --system-libs)

# target executable
TARGET = optimizer

# source files
SRCS = driver.cpp optimizer.cpp
OBJS = $(SRCS:.cpp=.o)

# ============================================================================
# BUILD RULES
# ============================================================================

# default target: build the optimizer
all: $(TARGET)

# link object files into executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS)

# compile .cpp files to .o files
%.o: %.cpp optimizer.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

# ============================================================================
# CLEAN
# ============================================================================

# remove all generated files
clean:
	rm -f $(TARGET) $(OBJS) *.ll.opt test_*.ll

# ============================================================================
# TESTING
# ============================================================================

# helper function to compare IR files ignoring metadata
# usage: make test_cfold_add
define compare_ir
	@echo "comparing with expected output..."
	@if diff -I '^; ModuleID' -I '^!1 = !{i32 [0-9]*, !"PIC Level"' $(1) $(2) > /dev/null 2>&1; then \
		echo "SUCCESS! ✓"; \
	else \
		echo "checking if only metadata differs..."; \
		if diff <(grep -v '^;' $(1) | grep -v '^!' | grep -v '^$$') \
		        <(grep -v '^;' $(2) | grep -v '^!' | grep -v '^$$') > /dev/null 2>&1; then \
			echo "SUCCESS! ✓ (metadata differences only)"; \
		else \
			echo "FAILED! ✗"; \
			echo "Differences found:"; \
			diff -u $(1) $(2) | head -30; \
		fi; \
	fi
endef

# test with cfold_add (constant folding test)
test_cfold_add: $(TARGET)
	@echo "=== testing constant folding (cfold_add) ==="
	@./$(TARGET) optimizer_test_results/cfold_add.ll > test_cfold_add.ll
	$(call compare_ir,optimizer_test_results/cfold_add_opt.ll,test_cfold_add.ll)

# test with cfold_mul (constant folding multiplication)
test_cfold_mul: $(TARGET)
	@echo "=== testing constant folding (cfold_mul) ==="
	@./$(TARGET) optimizer_test_results/cfold_mul.ll > test_cfold_mul.ll
	$(call compare_ir,optimizer_test_results/cfold_mul_opt.ll,test_cfold_mul.ll)

# test with cfold_sub (constant folding subtraction)
test_cfold_sub: $(TARGET)
	@echo "=== testing constant folding (cfold_sub) ==="
	@./$(TARGET) optimizer_test_results/cfold_sub.ll > test_cfold_sub.ll
	$(call compare_ir,optimizer_test_results/cfold_sub_opt.ll,test_cfold_sub.ll)

# test with p2 (common subexpression elimination)
test_cse: $(TARGET)
	@echo "=== testing common subexpression elimination (p2) ==="
	@./$(TARGET) optimizer_test_results/p2_common_subexpr.ll > test_cse.ll
	$(call compare_ir,optimizer_test_results/p2_common_subexpr_opt.ll,test_cse.ll)

# run all local optimization tests
test: test_cfold_add test_cfold_mul test_cfold_sub test_cse
	@echo ""
	@echo "=== ALL LOCAL OPTIMIZATION TESTS COMPLETE ==="

# quick test - just run one test to verify it works
quick: $(TARGET)
	@echo "=== quick test (cfold_add) ==="
	@./$(TARGET) optimizer_test_results/cfold_add.ll

# ============================================================================
# PHONY TARGETS
# ============================================================================

.PHONY: all clean test test_cfold_add test_cfold_mul test_cfold_sub test_cse quick