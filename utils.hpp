#ifndef UTILS_HPP
#define UTILS_HPP

#include <stdint.h>
#include <iostream>
#include <utility>

namespace utils {
	int64_t gcd(int64_t a, int64_t b);

	std::pair<int64_t, int64_t> stern_brocot(double d, double eps = 1e-10);
}

#endif
