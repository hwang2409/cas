# Minimal CAS implementation
```C++
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
```
> 390625

```C++
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
```
> -1
