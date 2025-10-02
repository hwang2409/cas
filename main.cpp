#include "edag.hpp"
#include "utils.hpp"
#include "rat.hpp"
#include <string>

int main() {
	eDAG t1, t2;

	t1.parse("cos(theta)");

	t2.parse("(2 + 3) / 5 * PI");

	double theta = t2.eval();

	std::cout << t1.eval({{"theta", theta}}) << "\n";

	eDAG t3 = t1.canonicalize();

	std::cout << t3.eval({{"theta", theta}}) << "\n";

	Rational p1{M_PI}, p2{M_PI};

	Rational p3 = p1 + p2;

	std::cout << p3.val() << "\n";

	std::cout << p3 << "\n";

	Rational p4{"-1/12"};

	std::cout << p4 << std::endl;

	return 0;
}
