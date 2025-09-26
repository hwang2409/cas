#include "expression_dag.h"
#include <algorithm>
#include <cmath>
#include <stack>
#include <queue>

// expression_node implementation
expression_node::expression_node(node_type t, const std::string& sym, double val, 
                               operation_type op, int prec, bool unary)
    : type(t), symbol(sym), value(val), operation(op), precedence(prec), is_unary(unary) {
}

std::string expression_node::to_string() const {
    switch (type) {
        case node_type::VARIABLE:
            return symbol;
        case node_type::CONSTANT:
            return std::to_string(value);
        case node_type::OPERATION:
            return symbol;
        case node_type::FUNCTION:
            return symbol + "()";
        default:
            return "unknown";
    }
}

double expression_node::evaluate(const std::unordered_map<std::string, double>& variables) const {
    switch (type) {
        case node_type::VARIABLE:
            {
                auto it = variables.find(symbol);
                if (it != variables.end()) {
                    return it->second;
                }
                throw std::runtime_error("Variable " + symbol + " not found in evaluation context");
            }
        case node_type::CONSTANT:
            return value;
        case node_type::OPERATION:
        case node_type::FUNCTION:
            throw std::runtime_error("Cannot evaluate operation node without operands");
        default:
            throw std::runtime_error("Unknown node type");
    }
}

bool expression_node::is_leaf() const {
    return type == node_type::VARIABLE || type == node_type::CONSTANT;
}

bool expression_node::is_operation() const {
    return type == node_type::OPERATION;
}

int expression_node::get_precedence() const {
    return precedence;
}

bool expression_node::is_left_associative() const {
    return operation == operation_type::ADD || operation == operation_type::SUBTRACT ||
           operation == operation_type::MULTIPLY || operation == operation_type::DIVIDE;
}

// expression_dag implementation
std::vector<std::string> expression_dag::tokenize(const std::string& expression) {
    std::vector<std::string> tokens;
    std::string current;
    
    for (size_t i = 0; i < expression.length(); ++i) {
        char c = expression[i];
        
        if (std::isspace(c)) {
            if (!current.empty()) {
                tokens.push_back(current);
                current.clear();
            }
        } else if (math_utils::is_operator(c) || c == '(' || c == ')') {
            if (!current.empty()) {
                tokens.push_back(current);
                current.clear();
            }
            tokens.push_back(std::string(1, c));
        } else {
            current += c;
        }
    }
    
    if (!current.empty()) {
        tokens.push_back(current);
    }
    
    return tokens;
}

std::vector<std::string> expression_dag::infix_to_postfix(const std::vector<std::string>& tokens) {
    std::vector<std::string> postfix;
    std::stack<std::string> operators;
    std::stack<int> precedences;
    
    for (size_t i = 0; i < tokens.size(); ++i) {
        const auto& token = tokens[i];
        
        if (math_utils::is_number(token)) {
            postfix.push_back(token);
        } else if (math_utils::string_to_operation(token) != operation_type::UNKNOWN) {
            // Function or named operator token (e.g., sin, cos)
            // Treat like an operator: push to operator stack
            operation_type op = math_utils::string_to_operation(token);
            int prec = math_utils::get_operation_precedence(op);
            while (!operators.empty() && operators.top() != "(" &&
                   ((precedences.top() > prec) ||
                    (precedences.top() == prec && math_utils::is_left_associative(op)))) {
                postfix.push_back(operators.top());
                operators.pop();
                precedences.pop();
            }
            operators.push(token);
            precedences.push(prec);
        } else if (token == "(") {
            operators.push(token);
            precedences.push(0);
        } else if (token == ")") {
            while (!operators.empty() && operators.top() != "(") {
                postfix.push_back(operators.top());
                operators.pop();
                precedences.pop();
            }
            if (!operators.empty()) {
                operators.pop();
                precedences.pop();
            }
            // If the token before '(' was a function, move it to output now
            if (!operators.empty()) {
                operation_type topOp = math_utils::string_to_operation(operators.top());
                if (topOp != operation_type::UNKNOWN && math_utils::is_unary_operation(topOp)) {
                    postfix.push_back(operators.top());
                    operators.pop();
                    if (!precedences.empty()) {
                        precedences.pop();
                    }
                }
            }
        } else if (math_utils::is_operator(token[0])) {
            operation_type op = math_utils::string_to_operation(token);
            int prec = math_utils::get_operation_precedence(op);
            
            while (!operators.empty() && operators.top() != "(" &&
                   ((precedences.top() > prec) || 
                    (precedences.top() == prec && math_utils::is_left_associative(op)))) {
                postfix.push_back(operators.top());
                operators.pop();
                precedences.pop();
            }
            
            operators.push(token);
            precedences.push(prec);
        } else {
            // Treat as variable/identifier
            postfix.push_back(token);
        }
    }
    
    while (!operators.empty()) {
        postfix.push_back(operators.top());
        operators.pop();
    }
    
    return postfix;
}

std::shared_ptr<expression_node> expression_dag::create_node(const std::string& token) {
    if (math_utils::is_number(token)) {
        double value = std::stod(token);
        return std::make_shared<expression_node>(node_type::CONSTANT, token, value);
    } else if (math_utils::string_to_operation(token) != operation_type::UNKNOWN) {
        // Check if it's a function/operation first
        operation_type op = math_utils::string_to_operation(token);
        int prec = math_utils::get_operation_precedence(op);
        bool isUnary = math_utils::is_unary_operation(op);
        return std::make_shared<expression_node>(node_type::OPERATION, token, 0.0, op, prec, isUnary);
    } else if (math_utils::is_variable(token)) {
        return std::make_shared<expression_node>(node_type::VARIABLE, token);
    } else {
        // Unknown token
        throw std::runtime_error("Unknown token: " + token);
    }
}

std::string expression_dag::generate_node_id() {
    static int counter = 0;
    return "node_" + std::to_string(++counter);
}

std::string expression_dag::intern_leaf(node_type t, const std::string& sym, double val) {
    std::string key = (t == node_type::VARIABLE ? "var:" + sym : "const:" + std::to_string(val));
    auto it = leaf_intern.find(key);
    if (it != leaf_intern.end()) return it->second;
    std::string id = generate_node_id();
    std::shared_ptr<expression_node> n;
    if (t == node_type::VARIABLE) {
        n = std::make_shared<expression_node>(node_type::VARIABLE, sym);
    } else {
        n = std::make_shared<expression_node>(node_type::CONSTANT, sym, val);
    }
    nodes[id] = n;
    graph.add_node(id);
    leaf_intern[key] = id;
    return id;
}

bool expression_dag::is_commutative(operation_type op) const {
    return op == operation_type::ADD || op == operation_type::MULTIPLY;
}

bool expression_dag::is_associative(operation_type op) const {
    return op == operation_type::ADD || op == operation_type::MULTIPLY;
}

std::string expression_dag::make_op_key(operation_type op, const std::vector<std::string>& child_ids) const {
    std::vector<std::string> ids = child_ids;
    if (is_commutative(op)) {
        std::vector<std::string> sorted = ids;
        std::sort(sorted.begin(), sorted.end());
        ids = sorted;
    }
    std::string key = math_utils::operation_to_string(op) + "|";
    for (size_t i = 0; i < ids.size(); ++i) {
        if (i) key += ",";
        key += ids[i];
    }
    return key;
}

std::string expression_dag::intern_op_node(operation_type op, const std::string& symbol, int precedence, bool is_unary,
                               const std::vector<std::string>& child_ids_ordered) {
    // For associative ops, flatten children using existing ordered_children
    std::vector<std::string> ordered = child_ids_ordered;
    if (is_associative(op)) {
        std::vector<std::string> flat;
        for (const auto& cid : ordered) {
            auto it = nodes.find(cid);
            if (it != nodes.end() && it->second->operation == op && !it->second->is_unary) {
                auto oit = ordered_children.find(cid);
                if (oit != ordered_children.end()) {
                    flat.insert(flat.end(), oit->second.begin(), oit->second.end());
                    continue;
                }
            }
            flat.push_back(cid);
        }
        ordered.swap(flat);
    }
    std::string key = make_op_key(op, ordered);
    auto it = op_intern.find(key);
    if (it != op_intern.end()) return it->second;
    std::string id = generate_node_id();
    auto node = std::make_shared<expression_node>(node_type::OPERATION, symbol, 0.0, op, precedence, is_unary);
    nodes[id] = node;
    graph.add_node(id);
    ordered_children[id] = ordered;
    for (const auto& cid : ordered) graph.add_edge(id, cid);
    op_intern[key] = id;
    return id;
}

void expression_dag::parse_expression(const std::string& expression) {
    clear();
    
    auto tokens = tokenize(expression);
    auto postfix = infix_to_postfix(tokens);
    
    std::stack<std::string> nodeStack;
    
        for (const auto& token : postfix) {
        if (math_utils::is_number(token)) {
            double v = std::stod(token);
            std::string nodeId = intern_leaf(node_type::CONSTANT, token, v);
            nodeStack.push(nodeId);
        } else if (math_utils::is_variable(token) && math_utils::string_to_operation(token) == operation_type::UNKNOWN) {
            // This is a pure variable, not a function
            std::string nodeId = intern_leaf(node_type::VARIABLE, token, 0.0);
            nodeStack.push(nodeId);
        } else {
            operation_type op = math_utils::string_to_operation(token);
            bool isUnary = math_utils::is_unary_operation(op);
            
            if (isUnary) {
                if (nodeStack.empty()) {
                    throw std::runtime_error("Invalid expression: unary operator without operand");
                }
                std::string operand = nodeStack.top();
                nodeStack.pop();
                
                std::string op_id = intern_op_node(op, token, math_utils::get_operation_precedence(op), true, {operand});
                nodeStack.push(op_id);
            } else {
                if (nodeStack.size() < 2) {
                    throw std::runtime_error("Invalid expression: binary operator without enough operands");
                }
                std::string right = nodeStack.top();
                nodeStack.pop();
                std::string left = nodeStack.top();
                nodeStack.pop();
                
                std::string op_id = intern_op_node(op, token, math_utils::get_operation_precedence(op), false, {left, right});
                nodeStack.push(op_id);
            }
        }
    }
    
    if (nodeStack.size() != 1) {
        throw std::runtime_error("Invalid expression: multiple root nodes");
    }
    
    root_node = nodeStack.top();
}

void expression_dag::add_variable(const std::string& name) {
    std::string nodeId = generate_node_id();
    auto node = std::make_shared<expression_node>(node_type::VARIABLE, name);
    nodes[nodeId] = node;
    graph.add_node(nodeId);
}

void expression_dag::add_constant(const std::string& name, double value) {
    std::string nodeId = generate_node_id();
    auto node = std::make_shared<expression_node>(node_type::CONSTANT, name, value);
    nodes[nodeId] = node;
    graph.add_node(nodeId);
}

void expression_dag::add_operation(const std::string& name, operation_type op, int precedence, bool is_unary) {
    std::string nodeId = generate_node_id();
    auto node = std::make_shared<expression_node>(node_type::OPERATION, name, 0.0, op, precedence, is_unary);
    nodes[nodeId] = node;
    graph.add_node(nodeId);
}

std::string expression_dag::get_root_node() const {
    return root_node;
}

double expression_dag::evaluate_node(const std::string& nodeId, 
                                   const std::unordered_map<std::string, double>& variables) const {
    auto nodeIt = nodes.find(nodeId);
    if (nodeIt == nodes.end()) {
        throw std::runtime_error("Node not found: " + nodeId);
    }
    
    auto node = nodeIt->second;
    
    if (node->is_leaf()) {
        return node->evaluate(variables);
    }
    
    // For operation nodes, evaluate operands first
    auto neighbors = graph.get_neighbors(nodeId);
    if (neighbors.empty()) {
        throw std::runtime_error("Operation node without operands: " + nodeId);
    }
    
    // Evaluate in preserved order if available
    const auto& child_order = ordered_children.find(nodeId);
    std::vector<std::string> eval_order;
    if (child_order != ordered_children.end() && !child_order->second.empty()) {
        eval_order = child_order->second;
    } else {
        eval_order = neighbors;
    }
    std::vector<double> operandValues;
    for (const auto& operand : eval_order) {
        operandValues.push_back(evaluate_node(operand, variables));
    }
    
    // Perform the operation
    switch (node->operation) {
        case operation_type::ADD:
            if (operandValues.size() != 2) throw std::runtime_error("ADD requires 2 operands");
            return operandValues[0] + operandValues[1];
        case operation_type::SUBTRACT:
            if (operandValues.size() != 2) throw std::runtime_error("SUBTRACT requires 2 operands");
            return operandValues[0] - operandValues[1];
        case operation_type::MULTIPLY:
            if (operandValues.size() != 2) throw std::runtime_error("MULTIPLY requires 2 operands");
            return operandValues[0] * operandValues[1];
        case operation_type::DIVIDE:
            if (operandValues.size() != 2) throw std::runtime_error("DIVIDE requires 2 operands");
            if (operandValues[1] == 0) throw std::runtime_error("Division by zero");
            return operandValues[0] / operandValues[1];
        case operation_type::POWER:
            if (operandValues.size() != 2) throw std::runtime_error("POWER requires 2 operands");
            return std::pow(operandValues[0], operandValues[1]);
        case operation_type::NEGATE:
            if (operandValues.size() != 1) throw std::runtime_error("NEGATE requires 1 operand");
            return -operandValues[0];
        case operation_type::SIN:
            if (operandValues.size() != 1) throw std::runtime_error("SIN requires 1 operand");
            return std::sin(operandValues[0]);
        case operation_type::COS:
            if (operandValues.size() != 1) throw std::runtime_error("COS requires 1 operand");
            return std::cos(operandValues[0]);
        case operation_type::TAN:
            if (operandValues.size() != 1) throw std::runtime_error("TAN requires 1 operand");
            return std::tan(operandValues[0]);
        case operation_type::LOG:
            if (operandValues.size() != 1) throw std::runtime_error("LOG requires 1 operand");
            return std::log(operandValues[0]);
        case operation_type::EXP:
            if (operandValues.size() != 1) throw std::runtime_error("EXP requires 1 operand");
            return std::exp(operandValues[0]);
        case operation_type::SQRT:
            if (operandValues.size() != 1) throw std::runtime_error("SQRT requires 1 operand");
            return std::sqrt(operandValues[0]);
        case operation_type::ABS:
            if (operandValues.size() != 1) throw std::runtime_error("ABS requires 1 operand");
            return std::abs(operandValues[0]);
        default:
            throw std::runtime_error("Unknown operation: " + node->symbol);
    }
}

double expression_dag::evaluate(const std::unordered_map<std::string, double>& variables) const {
    if (root_node.empty()) {
        throw std::runtime_error("No expression parsed");
    }
    return evaluate_node(root_node, variables);
}

std::vector<std::string> expression_dag::get_variables() const {
    std::vector<std::string> variables;
    for (const auto& pair : nodes) {
        if (pair.second->type == node_type::VARIABLE) {
            variables.push_back(pair.second->symbol);
        }
    }
    return variables;
}

std::vector<std::string> expression_dag::get_constants() const {
    std::vector<std::string> constants;
    for (const auto& pair : nodes) {
        if (pair.second->type == node_type::CONSTANT) {
            constants.push_back(pair.second->symbol);
        }
    }
    return constants;
}

std::vector<std::string> expression_dag::get_operations() const {
    std::vector<std::string> operations;
    for (const auto& pair : nodes) {
        if (pair.second->type == node_type::OPERATION) {
            operations.push_back(pair.second->symbol);
        }
    }
    return operations;
}

bool expression_dag::is_valid() const {
    return !root_node.empty() && !graph.has_cycle() && graph.size() > 0;
}

std::string expression_dag::to_string() const {
    if (root_node.empty()) {
        return "Empty expression";
    }
    
    // Simple recursive string building
    std::function<std::string(const std::string&)> buildString = [&](const std::string& nodeId) -> std::string {
        auto nodeIt = nodes.find(nodeId);
        if (nodeIt == nodes.end()) {
            return "unknown";
        }
        
        auto node = nodeIt->second;
        if (node->is_leaf()) {
            return node->to_string();
        }
        
        auto neighbors = graph.get_neighbors(nodeId);
        if (neighbors.empty()) {
            return node->symbol;
        }
        
        if (node->is_unary) {
            // unary: single ordered child if present
            const auto it = ordered_children.find(nodeId);
            const auto& child = (it != ordered_children.end() && !it->second.empty()) ? it->second[0] : neighbors[0];
            return node->symbol + "(" + buildString(child) + ")";
        } else {
            std::string result = "(";
            const auto it = ordered_children.find(nodeId);
            const auto& child_list = (it != ordered_children.end() && !it->second.empty()) ? it->second : neighbors;
            for (size_t i = 0; i < child_list.size(); ++i) {
                if (i > 0) result += " " + node->symbol + " ";
                result += buildString(child_list[i]);
            }
            result += ")";
            return result;
        }
    };
    
    return buildString(root_node);
}

std::string expression_dag::to_latex() const {
    std::function<std::string(const std::string&)> build = [&](const std::string& id) -> std::string {
        auto it = nodes.find(id);
        if (it == nodes.end()) return "?";
        const auto& n = it->second;
        if (n->is_leaf()) {
            if (n->type == node_type::VARIABLE) return n->symbol;
            std::ostringstream oss; oss << n->value; return oss.str();
        }
        auto itc = ordered_children.find(id);
        std::vector<std::string> ch = (itc != ordered_children.end()) ? itc->second : graph.get_neighbors(id);
        auto wrap = [&](const std::string& s){ return "(" + s + ")"; };
        switch (n->operation) {
            case operation_type::ADD: {
                std::string s;
                for (size_t i=0;i<ch.size();++i){ if(i) s += " + "; s += build(ch[i]); }
                return s;
            }
            case operation_type::MULTIPLY: {
                std::string s;
                for (size_t i=0;i<ch.size();++i){ if(i) s += " \\cdot "; s += build(ch[i]); }
                return s;
            }
            case operation_type::POWER: {
                std::string base = build(ch[0]);
                std::string exp = build(ch[1]);
                return base + "^{" + exp + "}";
            }
            case operation_type::SUBTRACT: {
                if (ch.size()==2) return build(ch[0]) + " - " + build(ch[1]);
                break;
            }
            case operation_type::DIVIDE: {
                if (ch.size()==2) return "\\frac{" + build(ch[0]) + "}{" + build(ch[1]) + "}";
                break;
            }
            case operation_type::NEGATE: {
                return "-" + wrap(build(ch[0]));
            }
            case operation_type::SIN: return "\\sin(" + build(ch[0]) + ")";
            case operation_type::COS: return "\\cos(" + build(ch[0]) + ")";
            case operation_type::TAN: return "\\tan(" + build(ch[0]) + ")";
            case operation_type::LOG: return "\\log(" + build(ch[0]) + ")";
            case operation_type::EXP: return "\\exp(" + build(ch[0]) + ")";
            case operation_type::SQRT: return "\\sqrt{" + build(ch[0]) + "}";
            case operation_type::ABS: return "|" + build(ch[0]) + "|";
            default: break;
        }
        return wrap(n->symbol);
    };
    return build(root_node);
}

expression_dag expression_dag::canonicalize() const {
    expression_dag out;
    std::function<std::string(const std::string&)> clone_id = [&](const std::string& id) -> std::string {
        auto it = nodes.find(id);
        if (it == nodes.end()) throw std::runtime_error("Node not found: " + id);
        const auto& n = it->second;
        if (n->is_leaf()) {
            if (n->type == node_type::VARIABLE) return out.intern_leaf(node_type::VARIABLE, n->symbol, 0.0);
            return out.intern_leaf(node_type::CONSTANT, std::to_string(n->value), n->value);
        }
        auto itc = ordered_children.find(id);
        std::vector<std::string> ch = (itc != ordered_children.end()) ? itc->second : graph.get_neighbors(id);
        std::vector<std::string> rebuilt;
        for (const auto& c : ch) rebuilt.push_back(clone_id(c));
        return out.intern_op_node(n->operation, n->symbol, n->precedence, n->is_unary, rebuilt);
    };
    out.root_node = clone_id(root_node);
    return out;
}

const dag<std::string>& expression_dag::get_graph() const {
    return graph;
}

std::shared_ptr<expression_node> expression_dag::get_node(const std::string& nodeId) const {
    auto it = nodes.find(nodeId);
    if (it != nodes.end()) {
        return it->second;
    }
    return nullptr;
}

void expression_dag::clear() {
    graph.clear();
    nodes.clear();
    root_node.clear();
    ordered_children.clear();
    leaf_intern.clear();
    op_intern.clear();
}

size_t expression_dag::size() const {
    return graph.size();
}

bool expression_dag::empty() const {
    return graph.empty();
}

void expression_dag::print_tree() const {
    std::cout << "Expression Tree:" << std::endl;
    graph.print();
    std::cout << "Root: " << root_node << std::endl;
}

expression_dag expression_dag::derivative(const std::string& variable) const {
    // Simplified derivative implementation
    // This is a basic version - a full implementation would be much more complex
    expression_dag result;
    // TODO: Implement proper symbolic differentiation
    return result;
}

expression_dag expression_dag::simplify() const {
    expression_dag out;
    std::function<std::string(const std::string&)> simplify_id = [&](const std::string& id) -> std::string {
        auto it = nodes.find(id);
        if (it == nodes.end()) throw std::runtime_error("Node not found: " + id);
        const auto& n = it->second;
        if (n->is_leaf()) {
            if (n->type == node_type::VARIABLE) return out.intern_leaf(node_type::VARIABLE, n->symbol, 0.0);
            return out.intern_leaf(node_type::CONSTANT, std::to_string(n->value), n->value);
        }
        std::vector<std::string> childs;
        auto oit = ordered_children.find(id);
        std::vector<std::string> child_ids = (oit != ordered_children.end()) ? oit->second : graph.get_neighbors(id);
        for (const auto& c : child_ids) childs.push_back(simplify_id(c));

        auto is_const_id = [&](const std::string& nid, double& val) -> bool {
            auto nit = out.nodes.find(nid);
            if (nit != out.nodes.end() && nit->second->type == node_type::CONSTANT) { val = nit->second->value; return true; }
            return false;
        };
        auto make_const = [&](double v){ return out.intern_leaf(node_type::CONSTANT, std::to_string(v), v); };

        switch (n->operation) {
            case operation_type::ADD: {
                std::vector<std::string> flat; double csum = 0.0;
                for (const auto& ch : childs) {
                    auto nit = out.nodes.find(ch);
                    if (nit != out.nodes.end() && nit->second->operation == operation_type::ADD && !nit->second->is_unary) {
                        auto fc = out.ordered_children.find(ch);
                        if (fc != out.ordered_children.end()) { flat.insert(flat.end(), fc->second.begin(), fc->second.end()); continue; }
                    }
                    double v; if (is_const_id(ch, v)) csum += v; else flat.push_back(ch);
                }
                if (csum != 0.0) flat.push_back(make_const(csum));
                if (flat.empty()) return make_const(0.0);
                if (flat.size() == 1) return flat[0];
                return out.intern_op_node(operation_type::ADD, "+", math_utils::get_operation_precedence(operation_type::ADD), false, flat);
            }
            case operation_type::MULTIPLY: {
                std::vector<std::string> flat; double cprod = 1.0;
                for (const auto& ch : childs) {
                    auto nit = out.nodes.find(ch);
                    if (nit != out.nodes.end() && nit->second->operation == operation_type::MULTIPLY && !nit->second->is_unary) {
                        auto fc = out.ordered_children.find(ch);
                        if (fc != out.ordered_children.end()) { flat.insert(flat.end(), fc->second.begin(), fc->second.end()); continue; }
                    }
                    double v; if (is_const_id(ch, v)) { if (v == 0.0) return make_const(0.0); cprod *= v; } else flat.push_back(ch);
                }
                if (cprod == 0.0) return make_const(0.0);
                if (cprod != 1.0) flat.push_back(make_const(cprod));
                if (flat.empty()) return make_const(1.0);
                if (flat.size() == 1) return flat[0];
                return out.intern_op_node(operation_type::MULTIPLY, "*", math_utils::get_operation_precedence(operation_type::MULTIPLY), false, flat);
            }
            case operation_type::SUBTRACT: {
                if (childs.size() == 2) {
                    double a, b; bool ca = is_const_id(childs[0], a), cb = is_const_id(childs[1], b);
                    if (cb && b == 0.0) return childs[0];
                    if (ca && cb) return make_const(a - b);
                }
                return out.intern_op_node(operation_type::SUBTRACT, "-", math_utils::get_operation_precedence(operation_type::SUBTRACT), false, childs);
            }
            case operation_type::DIVIDE: {
                if (childs.size() == 2) {
                    double a, b; bool ca = is_const_id(childs[0], a), cb = is_const_id(childs[1], b);
                    if (cb && b == 1.0) return childs[0];
                    if (ca && a == 0.0 && (!cb || b != 0.0)) return make_const(0.0);
                    if (ca && cb && b != 0.0) return make_const(a / b);
                }
                return out.intern_op_node(operation_type::DIVIDE, "/", math_utils::get_operation_precedence(operation_type::DIVIDE), false, childs);
            }
            case operation_type::POWER: {
                if (childs.size() == 2) {
                    double a, b; bool ca = is_const_id(childs[0], a), cb = is_const_id(childs[1], b);
                    if (cb && b == 1.0) return childs[0];
                    if (cb && b == 0.0) return make_const(1.0);
                    if (ca && cb) return make_const(std::pow(a, b));
                }
                return out.intern_op_node(operation_type::POWER, "^", math_utils::get_operation_precedence(operation_type::POWER), false, childs);
            }
            case operation_type::NEGATE: {
                if (childs.size() == 1) { double a; if (is_const_id(childs[0], a)) return make_const(-a); }
                return out.intern_op_node(operation_type::NEGATE, "-", math_utils::get_operation_precedence(operation_type::NEGATE), true, childs);
            }
            default: {
                if (childs.size() == 1) {
                    double a; if (is_const_id(childs[0], a)) {
                        double v;
                        switch (n->operation) {
                            case operation_type::SIN: v = std::sin(a); break;
                            case operation_type::COS: v = std::cos(a); break;
                            case operation_type::TAN: v = std::tan(a); break;
                            case operation_type::LOG: v = std::log(a); break;
                            case operation_type::EXP: v = std::exp(a); break;
                            case operation_type::SQRT: v = std::sqrt(a); break;
                            case operation_type::ABS: v = std::abs(a); break;
                            default: goto rebuild;
                        }
                        return make_const(v);
                    }
                }
            rebuild:
                return out.intern_op_node(n->operation, n->symbol, n->precedence, n->is_unary, childs);
            }
        }
    };
    out.root_node = simplify_id(root_node);
    return out;
}

expression_dag expression_dag::substitute(const std::unordered_map<std::string, std::string>& substitutions) const {
    // Simplified substitution
    // This is a basic version - a full implementation would be much more complex
    expression_dag result;
    // TODO: Implement proper symbolic substitution
    return result;
}

// math_utils implementation
namespace math_utils {
    operation_type string_to_operation(const std::string& op) {
        if (op == "+") return operation_type::ADD;
        if (op == "-") return operation_type::SUBTRACT;
        if (op == "*") return operation_type::MULTIPLY;
        if (op == "/") return operation_type::DIVIDE;
        if (op == "^") return operation_type::POWER;
        if (op == "sin") return operation_type::SIN;
        if (op == "cos") return operation_type::COS;
        if (op == "tan") return operation_type::TAN;
        if (op == "log") return operation_type::LOG;
        if (op == "exp") return operation_type::EXP;
        if (op == "sqrt") return operation_type::SQRT;
        if (op == "abs") return operation_type::ABS;
        return operation_type::UNKNOWN;
    }
    
    std::string operation_to_string(operation_type op) {
        switch (op) {
            case operation_type::ADD: return "+";
            case operation_type::SUBTRACT: return "-";
            case operation_type::MULTIPLY: return "*";
            case operation_type::DIVIDE: return "/";
            case operation_type::POWER: return "^";
            case operation_type::NEGATE: return "-";
            case operation_type::SIN: return "sin";
            case operation_type::COS: return "cos";
            case operation_type::TAN: return "tan";
            case operation_type::LOG: return "log";
            case operation_type::EXP: return "exp";
            case operation_type::SQRT: return "sqrt";
            case operation_type::ABS: return "abs";
            default: return "unknown";
        }
    }
    
    int get_operation_precedence(operation_type op) {
        switch (op) {
            case operation_type::ADD:
            case operation_type::SUBTRACT:
                return 1;
            case operation_type::MULTIPLY:
            case operation_type::DIVIDE:
                return 2;
            case operation_type::POWER:
                return 3;
            case operation_type::NEGATE:
            case operation_type::SIN:
            case operation_type::COS:
            case operation_type::TAN:
            case operation_type::LOG:
            case operation_type::EXP:
            case operation_type::SQRT:
            case operation_type::ABS:
                return 4;
            default:
                return 0;
        }
    }
    
    bool is_unary_operation(operation_type op) {
        return op == operation_type::NEGATE || op == operation_type::SIN || 
               op == operation_type::COS || op == operation_type::TAN ||
               op == operation_type::LOG || op == operation_type::EXP ||
               op == operation_type::SQRT || op == operation_type::ABS;
    }
    
    bool is_left_associative(operation_type op) {
        return op == operation_type::ADD || op == operation_type::SUBTRACT ||
               op == operation_type::MULTIPLY || op == operation_type::DIVIDE;
    }
    
    bool is_digit(char c) {
        return c >= '0' && c <= '9';
    }
    
    bool is_letter(char c) {
        return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
    }
    
    bool is_operator(char c) {
        return c == '+' || c == '-' || c == '*' || c == '/' || c == '^';
    }
    
    bool is_number(const std::string& str) {
        if (str.empty()) return false;
        size_t start = 0;
        if (str[0] == '-') start = 1;
        if (start >= str.length()) return false;
        
        bool hasDigit = false;
        bool hasDot = false;
        
        for (size_t i = start; i < str.length(); ++i) {
            if (is_digit(str[i])) {
                hasDigit = true;
            } else if (str[i] == '.' && !hasDot) {
                hasDot = true;
            } else {
                return false;
            }
        }
        
        return hasDigit;
    }
    
    bool is_variable(const std::string& str) {
        if (str.empty()) return false;
        
        for (char c : str) {
            if (!is_letter(c) && !is_digit(c) && c != '_') {
                return false;
            }
        }
        
        return is_letter(str[0]) || str[0] == '_';
    }
}
