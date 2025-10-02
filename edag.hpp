// Expression Directed Acyclic Graph
#ifndef EDAG_HPP
#define EDAG_HPP

#include "dag.hpp"
#include "rat.hpp"
#include <string>
#include <unordered_map>
#include <memory>
#include <variant>
#include <vector>
#include <sstream>
#include <stdexcept>

class eNode;
class eDAG;

enum class NodeType {
	VARIABLE, // x, y, z
	CONSTANT, // 1, 2, 3.14
	OPERATION, // +, -, *, /, sin
	FUNCTION // f(x), g(x,y)
};

enum class OPType {
	ADD,
	SUBTRACT,
	MULTIPLY,
	DIVIDE,
	POWER,
	NEGATE,
	SIN,
	COS,
	TAN,
	LOG,
	EXP,
	SQRT,
	ABS,
	UNKNOWN
};

class eNode {
	public:
		NodeType type;
		std::string symbol;
		std::variant<int64_t, Rational, double> value;
		OPType op;
		int precedence;
		bool is_unary;

		eNode(NodeType t,
			  const std::string &sym,
			  std::variant<int64_t, Rational, double> val = 0,
			  OPType op = OPType::UNKNOWN,
			  int prec = 0,
			  bool unary = false);

		std::string to_string() const;

		std::variant<int64_t, Rational, double> eval(const std::unordered_map<std::string, std::variant<int64_t, Rational, double>> &var = {}) const;

		bool is_leaf() const;

		bool is_op() const;

		int get_precedence() const;

		bool is_left_assoc() const;
};

class eDAG {
	private:
		DAG<std::string> graph;
		std::string root;
		std::unordered_map<std::string, std::shared_ptr<eNode>> nodes;
		std::unordered_map<std::string, std::vector<std::string>> orderedc;
		std::unordered_map<std::string, std::string> leaf_intern;
		std::unordered_map<std::string, std::string> op_intern;

		std::vector<std::string> tokenize(const std::string &expr);
		std::vector<std::string> infix2postfix(const std::vector<std::string> &tokens);
		std::shared_ptr<eNode> create_node(const std::string &token);
		std::string generate_id();
		std::string intern_leaf(NodeType t, const std::string &sym, std::variant<int64_t, Rational, double> val);
		bool is_comm(OPType op) const;
		bool is_assoc(OPType op) const;
		std::string make_op_key(OPType op, const std::vector<std::string> &children);
		std::string intern_op_node(OPType op,
								   const std::string &sym,
								   int precedence,
								   bool is_unary,
								   const std::vector<std::string> &children);
		std::variant<int64_t, Rational, double> eval_node(const std::string &node_id,
						 const std::unordered_map<std::string, std::variant<int64_t, Rational, double>> &var) const;

		std::vector<std::string> get_nodes(NodeType type) const;
	public:
		eDAG() = default;

		~eDAG() = default;

		// parse expression
		void parse(const std::string &exp);

		// add var node
		void add_var(const std::string &name);

		// add const node
		void add_const(const std::string &name, double val);

		// add operation node
		void add_op(const std::string &name,
					OPType op,
					int precedence,
					bool is_unary = 0);

		// return id of node
		std::string get_root() const;

		std::variant<int64_t, Rational, double> eval(const std::unordered_map<std::string, std::variant<int64_t, Rational, double>> &var = {}) const;
		
		// Add exact evaluation method
		std::variant<int64_t, Rational, double> eval_exact(const std::unordered_map<std::string, std::variant<int64_t, Rational, double>> &var = {}) const;

		std::vector<std::string> get_vars() const;

		std::vector<std::string> get_consts() const;

		std::vector<std::string> get_ops() const;

		bool is_valid() const;

		std::string to_string() const;

		std::string to_latex() const;

		const DAG<std::string>& get_graph() const;

		std::shared_ptr<eNode> get_node(const std::string& node_id) const;

		void clear();

		size_t size() const;

		bool empty() const;

		void tree() const;

		eDAG derivative(const std::string &var) const;

		eDAG simplify() const;

		eDAG canonicalize() const;

		eDAG substitute(const std::unordered_map<std::string, std::string> &subs) const;
		
		// Add rational detection and conversion
		bool is_rational_expression(const std::string &node_id) const;
		Rational to_rational(const std::string &node_id) const;
		Rational to_rational() const;
		
		// Add exact simplification methods
		eDAG simplify_exact() const;
		eDAG combine_like_terms() const;
		eDAG factor_rationals() const;
};

// Helper functions for variant handling
double variant_to_double(const std::variant<int64_t, Rational, double>& v);
bool all_rational(const std::vector<std::variant<int64_t, Rational, double>>& vals);
Rational variant_to_rational(const std::variant<int64_t, Rational, double>& v);

namespace math_utils {
	// convert op string to op type
	OPType string_to_op(const std::string &op);

	// convert op type to op string
	std::string op_to_string(OPType op);

	// get operation precedence
	int get_op_precedence(OPType op);

	// check if operation is unary
	bool is_unary(OPType op);

	// check if op is left associative
	bool is_left_assoc(OPType op);

	bool is_digit(char c);

	bool is_letter(char c);

	bool is_op(char c);

	bool is_num(const std::string &str);

	bool is_var(const std::string &str);
};

#include "edag.cpp"

#endif
