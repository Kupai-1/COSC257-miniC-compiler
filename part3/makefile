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

# test with cfold_add (constant folding test)
test_cfold_add: $(TARGET)
	@echo "=== testing constant folding (cfold_add) ==="
	./$(TARGET) optimizer_test_results/cfold_add.ll > test_cfold_add.ll
	@echo "comparing with expected output..."
	@diff -u optimizer_test_results/cfold_add_opt.ll test_cfold_add.ll && echo "SUCCESS!" || echo "DIFFERENCES FOUND"

# test with cfold_mul (constant folding multiplication)
test_cfold_mul: $(TARGET)
	@echo "=== testing constant folding (cfold_mul) ==="
	./$(TARGET) optimizer_test_results/cfold_mul.ll > test_cfold_mul.ll
	@echo "comparing with expected output..."
	@diff -u optimizer_test_results/cfold_mul_opt.ll test_cfold_mul.ll && echo "SUCCESS!" || echo "DIFFERENCES FOUND"

# test with cfold_sub (constant folding subtraction)
test_cfold_sub: $(TARGET)
	@echo "=== testing constant folding (cfold_sub) ==="
	./$(TARGET) optimizer_test_results/cfold_sub.ll > test_cfold_sub.ll
	@echo "comparing with expected output..."
	@diff -u optimizer_test_results/cfold_sub_opt.ll test_cfold_sub.ll && echo "SUCCESS!" || echo "DIFFERENCES FOUND"

# test with p2 (common subexpression elimination)
test_cse: $(TARGET)
	@echo "=== testing common subexpression elimination (p2) ==="
	./$(TARGET) optimizer_test_results/p2_common_subexpr.ll > test_cse.ll
	@echo "comparing with expected output..."
	@diff -u optimizer_test_results/p2_common_subexpr_opt.ll test_cse.ll && echo "SUCCESS!" || echo "DIFFERENCES FOUND"

# run all local optimization tests
test: test_cfold_add test_cfold_mul test_cfold_sub test_cse
	@echo ""
	@echo "=== ALL LOCAL OPTIMIZATION TESTS COMPLETE ==="

# quick test - just run one test to verify it works
quick: $(TARGET)
	@echo "=== quick test (cfold_add) ==="
	./$(TARGET) optimizer_test_results/cfold_add.ll

# ============================================================================
# PHONY TARGETS
# ============================================================================

.PHONY: all clean test test_cfold_add test_cfold_mul test_cfold_sub test_cse quick