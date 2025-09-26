CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
TARGET_DAG = test_dag
TARGET_EXPR = test_expression_dag
SOURCES_DAG = dag.cpp test_dag.cpp
SOURCES_EXPR = dag.cpp expression_dag.cpp test_expression_dag.cpp
HEADERS = dag.h expression_dag.h

# Default target
all: $(TARGET_DAG) $(TARGET_EXPR)

# Build the basic DAG test executable
$(TARGET_DAG): $(SOURCES_DAG) $(HEADERS)
	$(CXX) $(CXXFLAGS) -o $(TARGET_DAG) dag.cpp test_dag.cpp

# Build the expression DAG test executable
$(TARGET_EXPR): $(SOURCES_EXPR) $(HEADERS)
	$(CXX) $(CXXFLAGS) -o $(TARGET_EXPR) dag.cpp expression_dag.cpp test_expression_dag.cpp

# Run the basic DAG tests
test-dag: $(TARGET_DAG)
	./$(TARGET_DAG)

# Run the expression DAG tests
test-expr: $(TARGET_EXPR)
	./$(TARGET_EXPR)

# Run all tests
test: test-dag test-expr

# Clean build artifacts
clean:
	rm -f $(TARGET_DAG) $(TARGET_EXPR)

# Debug build
debug: CXXFLAGS += -g -DDEBUG
debug: $(TARGET_DAG) $(TARGET_EXPR)

# Release build
release: CXXFLAGS += -DNDEBUG
release: $(TARGET_DAG) $(TARGET_EXPR)

# Help target
help:
	@echo "Available targets:"
	@echo "  all        - Build all test executables (default)"
	@echo "  test-dag    - Build and run basic DAG tests"
	@echo "  test-expr   - Build and run expression DAG tests"
	@echo "  test        - Build and run all tests"
	@echo "  clean       - Remove build artifacts"
	@echo "  debug       - Build with debug symbols"
	@echo "  release     - Build optimized release version"
	@echo "  help        - Show this help message"

.PHONY: all test test-dag test-expr clean debug release help
