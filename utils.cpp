#include "utils.hpp"

namespace utils {
	int64_t gcd(int64_t a, int64_t b) {
		if (a < b) {
			return gcd(b, a);
		}

		if (b == 0)
			return a;

		return gcd(b, a % b);
	}

	std::pair<int64_t, int64_t> stern_brocot(double d, double eps) {
		if (d == 0.0) {
			return std::make_pair(0, 1);
		}

		if (d < 0) {
			return stern_brocot(-d);
		}

		int64_t left_num = 0, left_den = 1;
		int64_t right_num = 1, right_den = 0;

		while (1) {
			int64_t mid_num = left_num + right_num;
			int64_t mid_den = left_den + right_den;

			int64_t g = gcd(mid_num, mid_den);

			mid_num /= g;
			mid_den /= g;

			if (mid_den > 1000000) {
				break;
			}

			double mid_val = (double) mid_num / mid_den;

			if (std::abs(d - mid_val) < eps) {
				return std::make_pair(mid_num, mid_den);
			}

			if (d < mid_val) {
				right_num = mid_num;
				right_den = mid_den;
			} else {
				left_num = mid_num;
				left_den = mid_den;
			}
		}

		return std::make_pair(left_num, left_den);
	}
}
