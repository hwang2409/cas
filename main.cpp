#include "edag.hpp"
#include "utils.hpp"
#include "rat.hpp"
#include <string>
#include <iostream>
#include <unordered_map>

int main() {
	eDAG exact_tree;
	exact_tree.parse("1/3 + 2/3");

	try {
		auto result = exact_tree.eval_exact();
		if (std::holds_alternative<Rational>(result)) {
			std::cout << "Exact result: " << std::get<Rational>(result) << std::endl;
		} else if (std::holds_alternative<int64_t>(result)) {
			std::cout << "Exact result: " << std::get<int64_t>(result) << std::endl;
		} else {
			std::cout << "Approximate result: " << std::get<double>(result) << std::endl;
		}
	} catch (const std::exception& e) {
		std::cout << "Error: " << e.what() << std::endl;
	}

	eDAG var_tree;
	var_tree.parse("x + y");

	std::unordered_map<std::string, std::variant<int64_t, Rational, double>> vars;
	vars["x"] = Rational(1, 3);
	vars["y"] = Rational(2, 3);

	std::cout << "\nExpression: x + y where x=1/3, y=2/3" << std::endl;

	try {
		auto result = var_tree.eval(vars);
		if (std::holds_alternative<Rational>(result)) {
			std::cout << "Exact result: " << std::get<Rational>(result) << std::endl;
		} else if (std::holds_alternative<int64_t>(result)) {
			std::cout << "Exact result: " << std::get<int64_t>(result) << std::endl;
		} else {
			std::cout << "Approximate result: " << std::get<double>(result) << std::endl;
		}
	} catch (const std::exception& e) {
		std::cout << "Error: " << e.what() << std::endl;
	}

	std::cout << "\nRational detection tests:" << std::endl;

	eDAG rational_expr;
	rational_expr.parse("1/2 + 1/3");
	std::cout << "1/2 + 1/3 is rational: " << (rational_expr.is_rational_expression(rational_expr.get_root()) ? "true" : "false") << std::endl;

	eDAG mixed_expr;
	mixed_expr.parse("1/2 + sqrt(2)");
	std::cout << "1/2 + sqrt(2) is rational: " << (mixed_expr.is_rational_expression(mixed_expr.get_root()) ? "true" : "false") << std::endl;

	try {
		Rational r = rational_expr.to_rational();
		std::cout << "1/2 + 1/3 as rational: " << r << std::endl;
	} catch (const std::exception& e) {
		std::cout << "Cannot convert to rational: " << e.what() << std::endl;
	}

	std::cout << "\nExact simplification:" << std::endl;
	eDAG simplified = rational_expr.simplify_exact();
	std::cout << "Simplified expression (placeholder)" << std::endl;

	return 0;
}
