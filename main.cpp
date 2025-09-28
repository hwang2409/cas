#include "edag.hpp"
#include <string>

int main() {
	eDAG tree;

	tree.parse("abs(x + y) ^ 2 ^ 3");

	std::unordered_map<std::string, double> vars;

	vars["x"] = 5.0;
	vars["y"] = -10.0;

	std::cout << tree.eval(vars) << "\n";

	return 0;
}
