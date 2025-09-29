#include "edag.hpp"
#include <string>

int main() {
	eDAG t1, t2;

	t1.parse("cos(theta)");

	t2.parse("(2 + 3) / 5 * PI");

	double theta = t2.eval();

	std::cout << t1.eval({{"theta", theta}}) << "\n";

	return 0;
}
