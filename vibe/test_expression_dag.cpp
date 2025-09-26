#include "expression_dag.h"
#include <iostream>
#include <cassert>
#include <cmath>

void testBasicParsing() {
    std::cout << "=== Testing Basic Expression Parsing ===" << std::endl;
    
    expression_dag dag;
    
    // Test simple arithmetic
    dag.parse_expression("2 + 3");
    assert(dag.is_valid());
    assert(dag.size() == 3); // 2, 3, +
    
    double result = dag.evaluate();
    assert(std::abs(result - 5.0) < 1e-9);
    
    std::cout << "Expression: 2 + 3 = " << result << std::endl;
    std::cout << "String representation: " << dag.to_string() << std::endl;
    
    // Test with variables
    dag.parse_expression("x + y");
    assert(dag.is_valid());
    
    std::unordered_map<std::string, double> vars;
    vars["x"] = 5.0;
    vars["y"] = 3.0;
    result = dag.evaluate(vars);
    assert(std::abs(result - 8.0) < 1e-9);
    
    std::cout << "Expression: x + y = " << result << " (x=5, y=3)" << std::endl;
    
    std::cout << "Basic parsing test passed!" << std::endl;
}

void testComplexExpressions() {
    std::cout << "\n=== Testing Complex Expressions ===" << std::endl;
    
    expression_dag dag;
    
    // Test complex arithmetic
    dag.parse_expression("2 * 3 + 4 * 5");
    assert(dag.is_valid());
    
    double result = dag.evaluate();
    assert(std::abs(result - 26.0) < 1e-9);
    
    std::cout << "Expression: 2 * 3 + 4 * 5 = " << result << std::endl;
    std::cout << "String representation: " << dag.to_string() << std::endl;
    
    // Test with parentheses
    dag.parse_expression("(2 + 3) * 4");
    assert(dag.is_valid());
    
    result = dag.evaluate();
    assert(std::abs(result - 20.0) < 1e-9);
    
    std::cout << "Expression: (2 + 3) * 4 = " << result << std::endl;
    
    // Test power operation
    dag.parse_expression("2 ^ 3");
    assert(dag.is_valid());
    
    result = dag.evaluate();
    assert(std::abs(result - 8.0) < 1e-9);
    
    std::cout << "Expression: 2 ^ 3 = " << result << std::endl;
    
    std::cout << "Complex expressions test passed!" << std::endl;
}

void testMathematicalFunctions() {
    std::cout << "\n=== Testing Mathematical Functions ===" << std::endl;
    
    expression_dag dag;

    // Test sin function
    dag.parse_expression("sin(0)");
    assert(dag.is_valid());
    
    double result = dag.evaluate();
    assert(std::abs(result - 0.0) < 1e-9);
    std::cout << "Expression: sin(0) = " << result << std::endl;
    
    // Test cos function
    dag.parse_expression("cos(0)");
    assert(dag.is_valid());
    
    result = dag.evaluate();
    assert(std::abs(result - 1.0) < 1e-9);
    std::cout << "Expression: cos(0) = " << result << std::endl;
    
    // Test sqrt function
    dag.parse_expression("sqrt(16)");
    assert(dag.is_valid());
    
    result = dag.evaluate();
    assert(std::abs(result - 4.0) < 1e-9);
    std::cout << "Expression: sqrt(16) = " << result << std::endl;
    
    // Test log function
    dag.parse_expression("log(1)");
    assert(dag.is_valid());
    
    result = dag.evaluate();
    assert(std::abs(result - 0.0) < 1e-9);
    std::cout << "Expression: log(1) = " << result << std::endl;
    
    std::cout << "Mathematical functions test passed!" << std::endl;
}

void testVariableOperations() {
    std::cout << "\n=== Testing Variable Operations ===" << std::endl;
    
    expression_dag dag;
    
    // Test expression with multiple variables
    dag.parse_expression("x * y + z");
    assert(dag.is_valid());
    
    std::unordered_map<std::string, double> vars;
    vars["x"] = 2.0;
    vars["y"] = 3.0;
    vars["z"] = 4.0;
    
    double result = dag.evaluate(vars);
    assert(std::abs(result - 10.0) < 1e-9);
    
    std::cout << "Expression: x * y + z = " << result << " (x=2, y=3, z=4)" << std::endl;
    
    // Test getting variables
    auto variables = dag.get_variables();
    assert(variables.size() == 3);
    assert(std::find(variables.begin(), variables.end(), "x") != variables.end());
    assert(std::find(variables.begin(), variables.end(), "y") != variables.end());
    assert(std::find(variables.begin(), variables.end(), "z") != variables.end());
    
    std::cout << "Variables in expression: ";
    for (const auto& var : variables) {
        std::cout << var << " ";
    }
    std::cout << std::endl;
    
    std::cout << "Variable operations test passed!" << std::endl;
}

void testPrecedence() {
    std::cout << "\n=== Testing Operator Precedence ===" << std::endl;
    
    expression_dag dag;
    
    // Test multiplication before addition
    dag.parse_expression("2 + 3 * 4");
    assert(dag.is_valid());
    
    double result = dag.evaluate();
    assert(std::abs(result - 14.0) < 1e-9);
    
    std::cout << "Expression: 2 + 3 * 4 = " << result << std::endl;
    
    // Test power before multiplication
    dag.parse_expression("2 * 3 ^ 2");
    assert(dag.is_valid());
    
    result = dag.evaluate();
    assert(std::abs(result - 18.0) < 1e-9);
    
    std::cout << "Expression: 2 * 3 ^ 2 = " << result << std::endl;
    
    // Test parentheses override precedence
    dag.parse_expression("(2 + 3) * 4");
    assert(dag.is_valid());
    
    result = dag.evaluate();
    assert(std::abs(result - 20.0) < 1e-9);
    
    std::cout << "Expression: (2 + 3) * 4 = " << result << std::endl;
    
    std::cout << "Operator precedence test passed!" << std::endl;
}

void testErrorHandling() {
    std::cout << "\n=== Testing Error Handling ===" << std::endl;
    
    expression_dag dag;
    
    // Test invalid expression
    try {
        dag.parse_expression("2 +");
        assert(false); // Should not reach here
    } catch (const std::runtime_error& e) {
        std::cout << "Caught expected error: " << e.what() << std::endl;
    }
    
    // Test division by zero
    try {
        dag.parse_expression("1 / 0");
        assert(dag.is_valid());
        dag.evaluate(); // This should throw
        assert(false); // Should not reach here
    } catch (const std::runtime_error& e) {
        std::cout << "Caught expected division by zero error: " << e.what() << std::endl;
    }
    
    // Test undefined variable
    try {
        dag.parse_expression("x + y");
        assert(dag.is_valid());
        dag.evaluate(); // This should throw
        assert(false); // Should not reach here
    } catch (const std::runtime_error& e) {
        std::cout << "Caught expected undefined variable error: " << e.what() << std::endl;
    }
    
    std::cout << "Error handling test passed!" << std::endl;
}

void testDAGStructure() {
    std::cout << "\n=== Testing DAG Structure ===" << std::endl;
    
    expression_dag dag;
    
    dag.parse_expression("x + y * z");
    assert(dag.is_valid());
    
    // Test DAG properties
    assert(!dag.get_graph().has_cycle());
    assert(dag.get_graph().size() == 5); // x, y, z, *, +
    
    // Test root node
    std::string root = dag.get_root_node();
    assert(!root.empty());
    
    // Test node types
    auto rootNode = dag.get_node(root);
    assert(rootNode != nullptr);
    assert(rootNode->type == node_type::OPERATION);
    assert(rootNode->operation == operation_type::ADD);
    
    std::cout << "DAG structure test passed!" << std::endl;
}

void testComplexMathematicalExpression() {
    std::cout << "\n=== Testing Complex Mathematical Expression ===" << std::endl;
    
    expression_dag dag;
    
    // Test a complex expression without functions for now: (x^2 + y) * z
    dag.parse_expression("(x^2 + y) * z");
    assert(dag.is_valid());
    
    std::unordered_map<std::string, double> vars;
    vars["x"] = 2.0;  // x^2 = 4
    vars["y"] = 3.0;  // 4 + 3 = 7
    vars["z"] = 5.0;  // 7 * 5 = 35
    
    double result = dag.evaluate(vars);
    double expected = (2.0 * 2.0 + 3.0) * 5.0;  // (4 + 3) * 5 = 35
    assert(std::abs(result - expected) < 1e-9);
    
    std::cout << "Expression: (x^2 + y) * z = " << result << std::endl;
    std::cout << "Expected: " << expected << std::endl;
    std::cout << "String representation: " << dag.to_string() << std::endl;
    
    std::cout << "Complex mathematical expression test passed!" << std::endl;
}

void testPrintTree() {
    std::cout << "\n=== Testing Tree Printing ===" << std::endl;
    
    expression_dag dag;
    
    dag.parse_expression("(x + y) * (z - w)");
    assert(dag.is_valid());
    
    std::cout << "Expression tree structure:" << std::endl;
    dag.print_tree();
    
    std::cout << "Tree printing test completed!" << std::endl;
}

int main() {
    std::cout << "Starting Expression DAG Tests..." << std::endl;
    
    try {
        testBasicParsing();
        testComplexExpressions();
        testMathematicalFunctions();
        testVariableOperations();
        testPrecedence();
        testErrorHandling();
        testDAGStructure();
        testComplexMathematicalExpression();
        testPrintTree();
        
        std::cout << "\n=== All Expression DAG Tests Passed! ===" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
