#ifndef EXPRESSION_DAG_H
#define EXPRESSION_DAG_H

#include "dag.h"
#include <string>
#include <unordered_map>
#include <memory>
#include <variant>
#include <vector>
#include <sstream>
#include <stdexcept>

// Forward declarations
class expression_node;
class expression_dag;

// Node types for mathematical expressions
enum class node_type {
    VARIABLE,    // x, y, z, etc.
    CONSTANT,    // 1, 2, 3.14, etc.
    OPERATION,   // +, -, *, /, ^, sin, cos, etc.
    FUNCTION     // f(x), g(x,y), etc.
};

// Operation types
enum class operation_type {
    ADD,         // +
    SUBTRACT,    // -
    MULTIPLY,    // *
    DIVIDE,      // /
    POWER,       // ^
    NEGATE,      // unary -
    SIN,         // sin
    COS,         // cos
    TAN,         // tan
    LOG,         // log
    EXP,         // exp
    SQRT,        // sqrt
    ABS,         // abs
    UNKNOWN      // for parsing errors
};

// Expression node representing a mathematical component
class expression_node {
public:
    node_type type;
    std::string symbol;
    double value;
    operation_type operation;
    int precedence;
    bool is_unary;
    
    expression_node(node_type t, const std::string& sym, double val = 0.0, 
                    operation_type op = operation_type::UNKNOWN, int prec = 0, bool unary = false);
    
    // Get string representation
    std::string to_string() const;
    
    // Evaluate the node (for constants and variables with values)
    double evaluate(const std::unordered_map<std::string, double>& variables = {}) const;
    
    // Check if node is a leaf (variable or constant)
    bool is_leaf() const;
    
    // Check if node is an operation
    bool is_operation() const;
    
    // Get operation precedence
    int get_precedence() const;
    
    // Check if operation is left-associative
    bool is_left_associative() const;
};

// Expression DAG for computer algebra system
class expression_dag {
private:
    dag<std::string> graph;
    std::unordered_map<std::string, std::shared_ptr<expression_node>> nodes;
    std::string root_node;
    // Preserve operand order for each operation node
    std::unordered_map<std::string, std::vector<std::string>> ordered_children;
    // Interning for node sharing (hash-consing)
    // Key for leaves: type + symbol/value string
    std::unordered_map<std::string, std::string> leaf_intern;
    // Key for ops: op + '|' + joined child ids (ordered or sorted for commutative)
    std::unordered_map<std::string, std::string> op_intern;
    
    // Helper methods for parsing
    std::vector<std::string> tokenize(const std::string& expression);
    std::vector<std::string> infix_to_postfix(const std::vector<std::string>& tokens);
    std::shared_ptr<expression_node> create_node(const std::string& token);
    std::string generate_node_id();
    std::string intern_leaf(node_type t, const std::string& sym, double val);
    bool is_commutative(operation_type op) const;
    bool is_associative(operation_type op) const;
    std::string make_op_key(operation_type op, const std::vector<std::string>& child_ids) const;
    std::string intern_op_node(operation_type op, const std::string& symbol, int precedence, bool is_unary,
                               const std::vector<std::string>& child_ids_ordered);
    
    // Helper methods for evaluation
    double evaluate_node(const std::string& node_id, 
                         const std::unordered_map<std::string, double>& variables) const;
    
public:
    expression_dag() = default;
    ~expression_dag() = default;
    
    // Parse a mathematical expression string into DAG
    void parse_expression(const std::string& expression);
    
    // Add a variable node
    void add_variable(const std::string& name);
    
    // Add a constant node
    void add_constant(const std::string& name, double value);
    
    // Add an operation node
    void add_operation(const std::string& name, operation_type op, int precedence, bool is_unary = false);
    
    // Get the root node of the expression
    std::string get_root_node() const;
    
    // Evaluate the entire expression
    double evaluate(const std::unordered_map<std::string, double>& variables = {}) const;
    
    // Get all variables in the expression
    std::vector<std::string> get_variables() const;
    
    // Get all constants in the expression
    std::vector<std::string> get_constants() const;
    
    // Get all operations in the expression
    std::vector<std::string> get_operations() const;
    
    // Check if expression is valid (no cycles, all nodes connected)
    bool is_valid() const;
    
    // Get string representation of the expression
    std::string to_string() const;
    std::string to_latex() const;
    
    // Get the DAG structure
    const dag<std::string>& get_graph() const;
    
    // Get a specific node
    std::shared_ptr<expression_node> get_node(const std::string& node_id) const;
    
    // Clear the expression
    void clear();
    
    // Get the size (number of nodes)
    size_t size() const;
    
    // Check if empty
    bool empty() const;
    
    // Print the expression tree structure
    void print_tree() const;
    
    // Get derivative with respect to a variable
    expression_dag derivative(const std::string& variable) const;
    
    // Simplify the expression
    expression_dag simplify() const;
    expression_dag canonicalize() const;
    
    // Substitute variables with values or other expressions
    expression_dag substitute(const std::unordered_map<std::string, std::string>& substitutions) const;
};

// Utility functions for mathematical operations
namespace math_utils {
    // Convert operation string to OperationType
    operation_type string_to_operation(const std::string& op);
    
    // Convert OperationType to string
    std::string operation_to_string(operation_type op);
    
    // Get operation precedence
    int get_operation_precedence(operation_type op);
    
    // Check if operation is unary
    bool is_unary_operation(operation_type op);
    
    // Check if operation is left-associative
    bool is_left_associative(operation_type op);
    
    // Check if character is a digit
    bool is_digit(char c);
    
    // Check if character is a letter
    bool is_letter(char c);
    
    // Check if character is an operator
    bool is_operator(char c);
    
    // Check if string is a number
    bool is_number(const std::string& str);
    
    // Check if string is a variable name
    bool is_variable(const std::string& str);
}

#endif // EXPRESSION_DAG_H
