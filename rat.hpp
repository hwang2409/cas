#ifndef RAT_HPP
#define RAT_HPP

#include "utils.hpp"
#include <stdint.h>

class Rational {
	private:
		int64_t num, den;

		void normalize();
	public:
		Rational(int64_t num, int64_t den = 1);
		Rational(double d);
		Rational(const std::string &s);

		Rational operator+(const Rational &other) const;
		Rational operator-(const Rational &other) const;
		Rational operator*(const Rational &other) const;
		Rational operator/(const Rational &other) const;

		bool operator==(const Rational &other) const;
		bool operator!=(const Rational &other) const;
		bool operator<(const Rational &other) const;
		bool operator<=(const Rational &other) const;
		bool operator>(const Rational &other) const;
		bool operator>=(const Rational &other) const;

		double val() const;
		int64_t to_int() const;
		bool is_int() const;
		bool is_zero() const;

		int64_t numerator() const;
		int64_t denominator() const;

		std::pair<int64_t, int64_t> pair() const;

		friend std::ostream& operator<<(std::ostream& os, const Rational& r);
};

#endif
