# Expression DAG for Computer Algebra System

This project implements a Directed Acyclic Graph (DAG) specifically designed for parsing and evaluating mathematical expressions in a computer algebra system. The ExpressionDAG class extends the basic DAG functionality to handle mathematical operations, variables, constants, and functions.

## Features

### Core Expression Features
- **Mathematical Expression Parsing**: Parse string expressions into DAG representation
- **Variable Support**: Handle variables with runtime value substitution
- **Mathematical Functions**: Support for sin, cos, tan, log, exp, sqrt, abs
- **Operator Precedence**: Proper handling of mathematical operator precedence
- **Parentheses Support**: Full support for grouping expressions with parentheses
- **Error Handling**: Comprehensive error handling for invalid expressions

### Mathematical Operations
- **Arithmetic**: +, -, *, /, ^ (power)
- **Unary Operations**: -, sin, cos, tan, log, exp, sqrt, abs
- **Precedence**: Proper mathematical operator precedence
- **Associativity**: Left and right associativity support

### DAG Properties
- **Cycle Detection**: Prevents cycles in expression graphs
- **Topological Ordering**: Maintains proper evaluation order
- **Node Types**: Variables, constants, operations, and functions
- **Tree Structure**: Hierarchical representation of expressions

## Files

- `expression_dag.h` - Header file with ExpressionDAG class definition
- `expression_dag.cpp` - Implementation of ExpressionDAG methods
- `test_expression_dag.cpp` - Comprehensive test suite
- `dag.h` - Base DAG class (dependency)
- `dag.cpp` - Base DAG implementation (dependency)
- `Makefile` - Build configuration

## Building and Running

### Prerequisites
- C++17 compatible compiler (g++, clang++, etc.)
- Make utility

### Build Commands

```bash
# Build all components
make all

# Build and run expression DAG tests
make test-expr

# Build and run basic DAG tests
make test-dag

# Run all tests
make test

# Clean build artifacts
make clean

# Show help
make help
```

## Usage Examples

### Basic Expression Parsing

```cpp
#include "expression_dag.h"
#include <iostream>

int main() {
    ExpressionDAG dag;
    
    // Parse a simple expression
    dag.parseExpression("2 + 3 * 4");
    
    // Evaluate the expression
    double result = dag.evaluate();
    std::cout << "Result: " << result << std::endl; // Output: 14
    
    return 0;
}
```

### Variable Substitution

```cpp
#include "expression_dag.h"
#include <iostream>
#include <unordered_map>

int main() {
    ExpressionDAG dag;
    
    // Parse expression with variables
    dag.parseExpression("x^2 + y*2");
    
    // Set variable values
    std::unordered_map<std::string, double> variables;
    variables["x"] = 3.0;
    variables["y"] = 4.0;
    
    // Evaluate with variables
    double result = dag.evaluate(variables);
    std::cout << "Result: " << result << std::endl; // Output: 17 (3^2 + 4*2)
    
    return 0;
}
```

### Mathematical Functions

```cpp
#include "expression_dag.h"
#include <iostream>

int main() {
    ExpressionDAG dag;
    
    // Parse expression with functions
    dag.parseExpression("sin(x) + cos(y)");
    
    // Set variable values
    std::unordered_map<std::string, double> variables;
    variables["x"] = 0.0;  // sin(0) = 0
    variables["y"] = 0.0;  // cos(0) = 1
    
    // Evaluate
    double result = dag.evaluate(variables);
    std::cout << "Result: " << result << std::endl; // Output: 1
    
    return 0;
}
```

### Complex Expressions

```cpp
#include "expression_dag.h"
#include <iostream>

int main() {
    ExpressionDAG dag;
    
    // Parse complex expression
    dag.parseExpression("(x + y) * (z - w) / sqrt(a)");
    
    // Set variable values
    std::unordered_map<std::string, double> variables;
    variables["x"] = 5.0;
    variables["y"] = 3.0;
    variables["z"] = 10.0;
    variables["w"] = 2.0;
    variables["a"] = 16.0;  // sqrt(16) = 4
    
    // Evaluate
    double result = dag.evaluate(variables);
    std::cout << "Result: " << result << std::endl; // Output: 16 ((5+3)*(10-2)/4)
    
    return 0;
}
```

## API Reference

### ExpressionDAG Class

#### Core Methods
- `parseExpression(const std::string& expression)` - Parse a mathematical expression
- `evaluate(const std::unordered_map<std::string, double>& variables = {})` - Evaluate the expression
- `isValid()` - Check if the expression is valid
- `toString()` - Get string representation of the expression

#### Node Management
- `addVariable(const std::string& name)` - Add a variable node
- `addConstant(const std::string& name, double value)` - Add a constant node
- `addOperation(const std::string& name, OperationType op, int precedence, bool isUnary)` - Add an operation node

#### Analysis Methods
- `getVariables()` - Get all variables in the expression
- `getConstants()` - Get all constants in the expression
- `getOperations()` - Get all operations in the expression
- `getRootNode()` - Get the root node of the expression tree
- `getNode(const std::string& nodeId)` - Get a specific node

#### Utility Methods
- `size()` - Get the number of nodes
- `empty()` - Check if the expression is empty
- `clear()` - Clear the expression
- `printTree()` - Print the expression tree structure

### ExpressionNode Class

#### Properties
- `NodeType type` - Type of node (VARIABLE, CONSTANT, OPERATION, FUNCTION)
- `std::string symbol` - Symbol representation
- `double value` - Numeric value (for constants)
- `OperationType operation` - Operation type
- `int precedence` - Operator precedence
- `bool isUnary` - Whether the operation is unary

#### Methods
- `toString()` - Get string representation
- `evaluate(const std::unordered_map<std::string, double>& variables)` - Evaluate the node
- `isLeaf()` - Check if node is a leaf (variable or constant)
- `isOperation()` - Check if node is an operation

## Supported Mathematical Operations

### Arithmetic Operations
- `+` - Addition
- `-` - Subtraction
- `*` - Multiplication
- `/` - Division
- `^` - Power (exponentiation)

### Mathematical Functions
- `sin(x)` - Sine
- `cos(x)` - Cosine
- `tan(x)` - Tangent
- `log(x)` - Natural logarithm
- `exp(x)` - Exponential
- `sqrt(x)` - Square root
- `abs(x)` - Absolute value

### Operator Precedence
1. Parentheses: `()`
2. Unary operations: `-`, `sin`, `cos`, `tan`, `log`, `exp`, `sqrt`, `abs`
3. Power: `^` (right-associative)
4. Multiplication/Division: `*`, `/` (left-associative)
5. Addition/Subtraction: `+`, `-` (left-associative)

## Error Handling

The system provides comprehensive error handling for:

- **Invalid Expressions**: Malformed expressions, missing operands
- **Division by Zero**: Runtime detection of division by zero
- **Undefined Variables**: Variables not provided in evaluation context
- **Invalid Operations**: Operations with incorrect number of operands
- **Cycle Detection**: Prevention of cyclic dependencies

## Test Coverage

The test suite covers:

1. **Basic Parsing**: Simple arithmetic expressions
2. **Complex Expressions**: Multi-operation expressions with precedence
3. **Mathematical Functions**: Trigonometric and logarithmic functions
4. **Variable Operations**: Variable substitution and evaluation
5. **Operator Precedence**: Proper handling of mathematical precedence
6. **Error Handling**: Invalid expressions and runtime errors
7. **DAG Structure**: Graph properties and node relationships
8. **Complex Mathematical**: Real-world mathematical expressions

## Performance Characteristics

- **Parsing**: O(n) where n is the length of the expression
- **Evaluation**: O(V + E) where V is the number of nodes and E is the number of edges
- **Memory**: O(V + E) for storing the expression graph
- **Space**: Efficient adjacency list representation

## Future Enhancements

- **Symbolic Differentiation**: Automatic differentiation of expressions
- **Expression Simplification**: Algebraic simplification rules
- **Substitution**: Symbolic substitution of variables
- **Expression Comparison**: Structural comparison of expressions
- **LaTeX Output**: Mathematical notation output
- **More Functions**: Additional mathematical functions (asin, acos, atan, etc.)

## License

This implementation is provided as-is for educational and practical use in computer algebra systems.
