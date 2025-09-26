// Expression Directed Acyclic Graph
#ifndef EDAG_HPP
#define EDAG_HPP

#include "dag.h"
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
		OPType op;
		std::string symbol;
		double val;
		int precedence;
		bool is_unary;

		eNode(NodeType t,
			  const std::string &sym,
			  double val = 0.0,
			  OPType op = OPType::UNKNOWN,
			  int prec = 0,
			  bool unary = false);

		std::string to_string() const;

		double eval(const std::unordered_map<std::string, double> &var = {}) const;

		bool is_leaf() const;

		bool is_op() const;

		int get_precedence() const;

		bool is_left_associative() const;
};

class eDAG {
	private:
		DAG<std::string> graph;
};

#endif
