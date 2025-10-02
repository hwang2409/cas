#include "edag.hpp"
#include <algorithm>
#include <cmath>
#include <stack>
#include <queue>
#include <numbers>

eNode::eNode(NodeType t,
			 const std::string &sym,
			 double val,
			 OPType op,
			 int prec,
			 bool unary) : type(t),
						   symbol(sym),
						   val(val),
						   op(op),
						   precedence(prec),
						   is_unary(unary) {}

std::string eNode::to_string() const {
	switch (type) {
		case NodeType::VARIABLE:
			return symbol;
		case NodeType::CONSTANT:
			return std::to_string(val);
		case NodeType::OPERATION:
			return symbol;
		case NodeType::FUNCTION:
			return symbol + "()";
		default:
			return "UNKNOWN";
	}
}

double eNode::eval(const std::unordered_map<std::string, double> &var) const {
	switch (type) {
		case NodeType::VARIABLE:
			{
				std::unordered_map<std::string, double> constants = {
					{ "pi", M_PI },
					{ "PI", M_PI },
					{ "e", std::exp(1) },
					{ "tau", 2 * M_PI },
					{ "TAU", 2 * M_PI }
				};

				auto const_it = constants.find(symbol);
				if (const_it != constants.end()) {
					return const_it->second;
				}


				auto it = var.find(symbol);
				if (it != var.end()) {
					return it->second;
				}

				throw std::runtime_error("var: {" + symbol + "} not found in evaluation context.");
			}
		case NodeType::CONSTANT:
			return val;
		case NodeType::OPERATION:
		case NodeType::FUNCTION:
			throw std::runtime_error("can't evaluate op node without operands.");
		default:
			throw std::runtime_error("UNKNOWN NODE TYPE");
	}
}

bool eNode::is_leaf() const {
	return type == NodeType::VARIABLE || type == NodeType::CONSTANT;
}

bool eNode::is_op() const {
	return type == NodeType::OPERATION;
}

int eNode::get_precedence() const {
	return precedence;
}

bool eNode::is_left_assoc() const {
	return op == OPType::ADD|| op == OPType::SUBTRACT ||
		   op == OPType::MULTIPLY || op == OPType::DIVIDE;
}

std::vector<std::string> eDAG::tokenize(const std::string &expr) {
	std::vector<std::string> tokens;
	std::string curr;

	for (size_t j = 0; j < expr.length(); ++j) {
		char c = expr[j];

		if (std::isspace(c)) {
			if (!curr.empty()) {
				tokens.push_back(curr);
				curr.clear();
			}
		} else if (math_utils::is_op(c) || c == '(' || c == ')') {
			if (!curr.empty()) {
				tokens.push_back(curr);
				curr.clear();
			}

			tokens.push_back(std::string(1,c));
		} else {
			curr += c;
		}
	}

	if (!curr.empty())
		tokens.push_back(curr);

	return tokens;
}

std::vector<std::string> eDAG::infix2postfix(const std::vector<std::string> &tokens) {
	std::vector<std::string> pfix;
	std::stack<std::string> ops;
	std::stack<int> precedences;

	for (size_t j = 0; j < tokens.size(); ++j) {
		const auto &token = tokens[j];
		std::string current = token;

		// Detect unary minus and convert to explicit NEGATE operator token
		// Unary if at expression start, after '(', or after another operator
		if (current == "-") {
			bool is_unary_context = (j == 0);
			if (!is_unary_context) {
				const std::string &prev = tokens[j - 1];
				is_unary_context = (prev == "(" || math_utils::string_to_op(prev) != OPType::UNKNOWN);
			}
			if (is_unary_context) {
				current = "neg"; // map to NEGATE
			}
		}

		if (math_utils::is_num(current)) {
			pfix.push_back(current);
		} else if (math_utils::string_to_op(current) != OPType::UNKNOWN) {
			OPType op = math_utils::string_to_op(current);
			int prec = math_utils::get_op_precedence(op);

			while (!ops.empty() && ops.top() != "(" &&
					((!precedences.empty() &&
					 ((precedences.top() > prec) ||
					 (precedences.top() == prec &&
					  math_utils::is_left_assoc(op)))))) {
				pfix.push_back(ops.top());
				ops.pop();
				precedences.pop();
			}

			ops.push(current);
			precedences.push(prec);
		} else if (current == "(") {
			ops.push(token);
			precedences.push(0);
		} else if (current == ")") {
			while (!ops.empty() && ops.top() != "(") {
				pfix.push_back(ops.top());
				ops.pop();
				precedences.pop();
			}

			if (!ops.empty()) {
				ops.pop();
				precedences.pop();
			}

			if (!ops.empty()) {
				OPType op = math_utils::string_to_op(ops.top());
				if (op != OPType::UNKNOWN && math_utils::is_unary(op)) {
					pfix.push_back(ops.top());
					ops.pop();

					if (!precedences.empty()) {
						precedences.pop();
					}
				}
			}
		} else {
			pfix.push_back(current);
		}
	}

	while (!ops.empty()) {
		pfix.push_back(ops.top());
		ops.pop();
	}

	return pfix;
}

std::shared_ptr<eNode> eDAG::create_node(const std::string &token) {
	if (math_utils::is_num(token)) {
		double val = std::stod(token);
		return std::make_shared<eNode>(NodeType::CONSTANT,
									   token,
									   val);
	} else if (math_utils::string_to_op(token) != OPType::UNKNOWN) {
		OPType op = math_utils::string_to_op(token);
		int prec = math_utils::get_op_precedence(op);
		bool is_unary = math_utils::is_unary(op);

		return std::make_shared<eNode>(NodeType::OPERATION,
									   token,
									   0.0,
									   op,
									   prec,
									   is_unary);
	} else if (math_utils::is_var(token)) {
		return std::make_shared<eNode>(NodeType::VARIABLE,
									   token);
	} else {
		throw std::runtime_error("UNKNOWN TOKEN: " + token);
	}
}

std::string eDAG::generate_id() {
	static int counter = 0;
	return "node_" + std::to_string(++counter);
}

std::string eDAG::intern_leaf(NodeType t,
							  const std::string &sym,
							  double val) {
	std::string key = (t == NodeType::VARIABLE ? "var:" + sym
											   : "const:" + std::to_string(val));

	auto it = leaf_intern.find(key);

	if (it != leaf_intern.end())
		return it->second;

	std::string id = generate_id();
	std::shared_ptr<eNode> node;

	if (t == NodeType::VARIABLE) {
		node = std::make_shared<eNode>(NodeType::VARIABLE, sym);
	} else {
		node = std::make_shared<eNode>(NodeType::CONSTANT, sym, val);
	}

	nodes[id] = node;
	graph.add_node(id);
	leaf_intern[key] = id;
	return id;
}

bool eDAG::is_comm(OPType op) const {
	return (op == OPType::ADD || op == OPType::MULTIPLY);
}

bool eDAG::is_assoc(OPType op) const {
	return (op == OPType::ADD || op == OPType::MULTIPLY);
}

std::string eDAG::make_op_key(OPType op, const std::vector<std::string> &children) {
	std::vector<std::string> ids = children;

	if (this->is_comm(op)) {
		std::vector<std::string> sorted = ids;
		std::sort(sorted.begin(), sorted.end());
		ids = sorted;
	}

	std::string key = math_utils::op_to_string(op) + "|";

	for (size_t j = 0; j < ids.size(); ++j) {
		if (j)
			key += ",";

		key += ids[j];
	}

	return key;
}

std::string eDAG::intern_op_node(OPType op,
								 const std::string &sym,
								 int precedence,
								 bool is_unary,
								 const std::vector<std::string> &children) {
	std::vector<std::string> ordered = children;

	if (this->is_assoc(op)) {
		std::vector<std::string> flat;

		for (const auto &child_id : ordered) {
			auto it = nodes.find(child_id);
			if (it != nodes.end() && it->second->op == op && !it->second->is_unary) {
				auto ordered_it = orderedc.find(child_id);
				if (ordered_it != orderedc.end()) {
					flat.insert(flat.end(),
							    ordered_it->second.begin(),
								ordered_it->second.end());
					continue;
				}
			}
			flat.push_back(child_id);
		}
		ordered.swap(flat);
	}

	std::string key = this->make_op_key(op, ordered);

	auto it = op_intern.find(key);

	if (it != op_intern.end())
		return it->second;

	std::string id = this->generate_id();

	auto node = std::make_shared<eNode>(NodeType::OPERATION,
										sym,
										0.0,
										op,
										precedence,
										is_unary);

	nodes[id] = node;
	graph.add_node(id);
	orderedc[id] = ordered;

	for (const auto &child_id : ordered)
		graph.add_edge(id, child_id);

	op_intern[key] = id;

	return id;
}

double eDAG::eval_node(const std::string &node_id,
				 const std::unordered_map<std::string, double> &var) const {
	auto node_it = nodes.find(node_id);

	if (node_it == nodes.end()) {
		throw std::runtime_error("node not found: " + node_id);
	}

	auto node = node_it->second;

	if (node->is_leaf()) {
		return node->eval(var);
	}

	auto neighbors = graph.get_neighbors(node_id);

	if (neighbors.empty()) {
		throw std::runtime_error("operation node without operands: " + node_id);
	}

	const auto &child_order = orderedc.find(node_id);

	std::vector<std::string> eval_order;

	if (child_order != orderedc.end() && !child_order->second.empty()) {
		eval_order = child_order->second;
	} else {
		eval_order = neighbors;
	}

	std::vector<double> op_vals;

	for (const auto &op : eval_order) {
		op_vals.push_back(this->eval_node(op, var));
	}

	switch (node->op) {
		case (OPType::ADD): {
			if (op_vals.empty()) throw std::runtime_error("ADD requires >=1 op.");
			double s = op_vals[0];
			for (size_t j = 1; j < op_vals.size(); ++j) s += op_vals[j];
			return s;
		}
		case (OPType::SUBTRACT):
			if (op_vals.size() != 2) throw std::runtime_error("SUB requires 2 ops.");
			return op_vals[0] - op_vals[1];
		case (OPType::MULTIPLY): {
			if (op_vals.empty()) throw std::runtime_error("MUL requires >=1 op.");
			double p = op_vals[0];
			for (size_t j = 1; j < op_vals.size(); ++j) p *= op_vals[j];
			return p;
		}
		case (OPType::DIVIDE):
			if (op_vals.size() != 2) throw std::runtime_error("DIV requires 2 ops.");
			if (op_vals[1] == 0) throw std::runtime_error("DIV BY ZERO.");
			return op_vals[0] / op_vals[1];
		case (OPType::POWER):
			if (op_vals.size() != 2) throw std::runtime_error("POW requires 2 ops.");
			return std::pow(op_vals[0], op_vals[1]);
		case (OPType::NEGATE):
			if (op_vals.size() != 1) throw std::runtime_error("NEG requires 1 op.");
			return -op_vals[0];
		case (OPType::SIN):
			if (op_vals.size() != 1) throw std::runtime_error("SIN requires 1 op.");
			return std::sin(op_vals[0]);
		case (OPType::COS):
			if (op_vals.size() != 1) throw std::runtime_error("COS requires 1 op.");
			return std::cos(op_vals[0]);
		case (OPType::TAN):
			if (op_vals.size() != 1) throw std::runtime_error("TAN requires 1 op.");
			return std::tan(op_vals[0]);
		case (OPType::LOG):
			if (op_vals.size() != 1) throw std::runtime_error("LOG requires 1 op.");
			return std::log(op_vals[0]);
		case (OPType::EXP):
			if (op_vals.size() != 1) throw std::runtime_error("EXP requires 1 op.");
			return std::exp(op_vals[0]);
		case (OPType::SQRT):
			if (op_vals.size() != 1) throw std::runtime_error("SQRT requires 1 op.");
			return std::sqrt(op_vals[0]);
		case (OPType::ABS):
			if (op_vals.size() != 1) throw std::runtime_error("ABS requires 1 op.");
			return std::abs(op_vals[0]);
		default:
			throw std::runtime_error("UNKNOWN OP: " + node->symbol);
	}
}

std::vector<std::string> eDAG::get_nodes(NodeType type) const {
	std::vector<std::string> filter;

	for (const auto &p : nodes) {
		if (p.second->type == type) {
			filter.push_back(p.second->symbol);
		}
	}

	return filter;
}

void eDAG::parse(const std::string &expr) {
	clear();

	auto tokens = this->tokenize(expr);
	auto postfix = this->infix2postfix(tokens);

	std::stack<std::string> node_stack;

	for (const auto &token : postfix) {
		if (math_utils::is_num(token)) {
			double val = std::stod(token);
			std::string node_id = intern_leaf(NodeType::CONSTANT,
											  token,
											  val);
			node_stack.push(node_id);
		} else if (math_utils::is_var(token) &&
				   math_utils::string_to_op(token) == OPType::UNKNOWN) {
			std::string node_id = intern_leaf(NodeType::VARIABLE,
											  token,
											  0.0);
			node_stack.push(node_id);
		} else {
			OPType op = math_utils::string_to_op(token);
			bool is_unary = math_utils::is_unary(op);

			if (is_unary) {
				if (node_stack.empty()) {
					throw std::runtime_error("invalid expr: unary op without operand");
				}
				std::string arg = node_stack.top();
				node_stack.pop();

				std::string op_id = intern_op_node(op,
												   token,
												   math_utils::get_op_precedence(op),
												   1,
												   { arg });
				node_stack.push(op_id);
			} else {
				if (node_stack.size() < 2) {
					throw std::runtime_error("invalid expr: op without operands");
				}

				std::string right = node_stack.top();
				node_stack.pop();
				std::string left = node_stack.top();
				node_stack.pop();

				std::string op_id = intern_op_node(op,
												   token,
												   math_utils::get_op_precedence(op),
												   0,
												   { left, right });

				node_stack.push(op_id);
			}
		}
	}

	if (node_stack.size() != 1) {
		throw std::runtime_error("invalid expression: multiple root nodes.");
	}

	root = node_stack.top();
}

void eDAG::add_var(const std::string &name) {
	std::string node_id = this->generate_id();

	auto node = std::make_shared<eNode>(NodeType::VARIABLE,
										name);

	nodes[node_id] = node;
	graph.add_node(node_id);
}

void eDAG::add_const(const std::string &name, double value) {
	std::string node_id = this->generate_id();

	auto node = std::make_shared<eNode>(NodeType::CONSTANT,
										name,
										value);

	nodes[node_id] = node;
	graph.add_node(node_id);
}

void eDAG::add_op(const std::string &name,
				  OPType op,
				  int precedence,
				  bool is_unary) {
	std::string node_id = this->generate_id();

	auto node = std::make_shared<eNode>(NodeType::OPERATION,
										name,
										0.0,
										op,
										precedence,
										is_unary);

	nodes[node_id] = node;
	graph.add_node(node_id);
}

std::string eDAG::get_root() const {
	return root;
}

double eDAG::eval(const std::unordered_map<std::string, double> &var) const {
	if (root.empty()) {
		throw std::runtime_error("no expression parsed.");
	}

	return eval_node(root, var);
}

std::vector<std::string> eDAG::get_vars() const {
	return this->get_nodes(NodeType::VARIABLE);
}

std::vector<std::string> eDAG::get_consts() const {
	return this->get_nodes(NodeType::CONSTANT);
}

std::vector<std::string> eDAG::get_ops() const {
	return this->get_nodes(NodeType::OPERATION);
}

bool eDAG::is_valid() const {
	return !root.empty() && !graph.has_cycle() && graph.size() > 0;
}

// TODO
std::string eDAG::to_string() const {
	return "";
}

// TODO
std::string eDAG::to_latex() const {
	return "";
}

const DAG<std::string>& eDAG::get_graph() const {
	return graph;
}

std::shared_ptr<eNode> eDAG::get_node(const std::string &node_id) const {
	auto it = nodes.find(node_id);

	if (it != nodes.end()) {
		return it->second;
	}

	return nullptr;
}

void eDAG::clear() {
	graph.clear();
	nodes.clear();
	root.clear();
	orderedc.clear();
	leaf_intern.clear();
	op_intern.clear();
}

size_t eDAG::size() const {
	return graph.size();
}

bool eDAG::empty() const {
	return (graph.size() == 0);
}

eDAG eDAG::canonicalize() const {
	eDAG out;

	std::function<std::string(const std::string&)> clone_id = [&](const std::string &id) -> std::string {
		auto it = nodes.find(id);

		if (it == nodes.end()) {
			throw std::runtime_error("node not found: " + id);
		}

		const auto &node = it->second;

		if (node->is_leaf()) {
			if (node->type == NodeType::VARIABLE) {
				return out.intern_leaf(NodeType::VARIABLE, node->symbol, 0.0);
			} else {
				return out.intern_leaf(NodeType::CONSTANT, std::to_string(node->val), node->val);
			}
		}

		auto itc = orderedc.find(id);

		std::vector<std::string> children = ((itc != orderedc.end()) ? itc->second : graph.get_neighbors(id));
		std::vector<std::string> rebuilt;

		for (const auto &c : children) {
			rebuilt.push_back(clone_id(c));
		}

		return out.intern_op_node(node->op,
								  node->symbol,
								  node->precedence,
								  node->is_unary,
								  rebuilt);
	};

	out.root = clone_id(this->root);

	return out;
}

namespace math_utils {
	OPType string_to_op(const std::string &op) {
		if (op == "+") return OPType::ADD;
		if (op == "-") return OPType::SUBTRACT;
		if (op == "neg") return OPType::NEGATE;
		if (op == "*") return OPType::MULTIPLY;
		if (op == "/") return OPType::DIVIDE;
		if (op == "^") return OPType::POWER;
		if (op == "sin") return OPType::SIN;
		if (op == "cos") return OPType::COS;
		if (op == "tan") return OPType::TAN;
		if (op == "log") return OPType::LOG;
		if (op == "exp") return OPType::EXP;
		if (op == "sqrt") return OPType::SQRT;
		if (op == "abs") return OPType::ABS;
		return OPType::UNKNOWN;
	}

	std::string op_to_string(OPType op) {
		switch (op) {
			case OPType::ADD: return "+";
			case OPType::SUBTRACT: return "-";
			case OPType::MULTIPLY: return "*";
			case OPType::DIVIDE: return "/";
			case OPType::POWER: return "^";
			case OPType::NEGATE: return "-";
			case OPType::SIN: return "sin";
			case OPType::COS: return "cos";
			case OPType::TAN: return "tan";
			case OPType::LOG: return "log";
			case OPType::EXP: return "exp";
			case OPType::SQRT: return "sqrt";
			case OPType::ABS: return "abs";
			default: return "UNKNOWN";
		}
	}

	int get_op_precedence(OPType op) {
		switch (op) {
			case OPType::ADD:
			case OPType::SUBTRACT:
				return 1;
			case OPType::MULTIPLY:
			case OPType::DIVIDE:
				return 2;
			case OPType::POWER:
				return 3;
			case OPType::NEGATE:
			case OPType::SIN:
			case OPType::COS:
			case OPType::TAN:
			case OPType::LOG:
			case OPType::EXP:
			case OPType::SQRT:
			case OPType::ABS:
				return 4;
			default:
				return 0;
		}
	}

	bool is_unary(OPType op) {
		return op == OPType::NEGATE || op == OPType::SIN ||
			   op == OPType::COS || op == OPType::TAN ||
			   op == OPType::LOG || op == OPType::EXP ||
			   op == OPType::SQRT || op == OPType::ABS;
	}

	bool is_left_assoc(OPType op) {
		return op == OPType::ADD || op == OPType::SUBTRACT ||
			   op == OPType::MULTIPLY || op == OPType::DIVIDE;
	}

	bool is_right_assoc(OPType op) {
		return op == OPType::POWER;
	}

	bool is_digit(char c) {
		return c >= '0' && c <= '9';
	}

	bool is_letter(char c) {
		return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
	}

	bool is_op(char c) {
		return (c == '+' || c == '-' || c == '*' || c == '/' || c == '^');
	}

	bool is_num(const std::string &str) {
		if (str.empty())
			return 0;

		size_t start = (str[0] == '-');

		if (start >= str.length())
			return 0;

		bool has_digit = 0,
			 has_dot = 0;

		for (size_t j = start; j < str.length(); ++j) {
			if (is_digit(str[j])) {
				has_digit = 1;
			} else if (str[j] == '.' && !has_dot) {
				has_dot = 1;
			} else {
				return 0;
			}
		}

		return has_digit;
	}

	bool is_var(const std::string &str) {
		if (str.empty())
			return 0;

		for (char c : str) {
			if (!is_letter(c) && !is_digit(c) && c != '_')
				return 0;
		}

		return (is_letter(str[0]) || str[0] == '_');
	}
};
