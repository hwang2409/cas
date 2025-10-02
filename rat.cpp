#include "rat.hpp"
#include <string>
#include <algorithm>

void Rational::normalize() {
	int64_t gcd = utils::gcd(std::abs(num), std::abs(den));

	this->num /= gcd;
	this->den /= gcd;
}

Rational::Rational(int64_t num, int64_t den) {
	if (num < 0 && den < 0) {
		this->num = -num;
		this->den = -den;
	} else if ((num < 0 && den > 0) || (num > 0 && den < 0)) {
		this->num = -std::abs(num);
		this->den = std::abs(den);
	} else {
		this->num = num;
		this->den = den;
	}

	this->normalize();
}

Rational::Rational(const std::string &copy) {
	std::string s = copy;
	s.erase(std::remove(s.begin(), s.end(), ' '), s.end());

	bool neg = (s[0] == '-');

	if (neg) {
		s = s.substr(1);
	}

	size_t split = s.find('/');

	if (split == std::string::npos) {
		this->num = (int64_t) std::stoll(s);
		this->den = 1;
	} else {
		this->num = (int64_t) std::stoll(s.substr(0, split));
		this->den = (int64_t) std::stoll(s.substr(split+1));

		if (neg)
			this->num *= -1;

		this->normalize();
	}
}

Rational::Rational(double d) {
	std::pair<int64_t, int64_t> p = utils::stern_brocot(d);

	this->num = p.first;
	this->den = p.second;

	this->normalize();
}

// a/b + c/d
// ad/bd + bc/bd
// (ad + bc)/bd
Rational Rational::operator+(const Rational &other) const {
	int64_t a = this->num,
			b = this->den,
			c = other.numerator(),
			d = other.denominator();

	int64_t n1, n2, n3, d1;

	if (__builtin_mul_overflow(a, d, &n1) ||
		__builtin_mul_overflow(b, c, &n2) ||
		__builtin_mul_overflow(b, d, &d1)) {
		throw std::runtime_error("MUL overflow.");
	}

	if (__builtin_add_overflow(n1, n2, &n3)) {
		throw std::runtime_error("ADD overflow.");
	}

	return Rational{n3, d1};
}

// a/b - c/d
// ad/bd - bc/bd
// (ad - bc)/bd
Rational Rational::operator-(const Rational &other) const {
	int64_t a = this->num,
			b = this->den,
			c = other.numerator(),
			d = other.denominator();

	int64_t n1, n2, n3, d1;

	if (__builtin_mul_overflow(a, d, &n1) ||
		__builtin_mul_overflow(b, c, &n2) ||
		__builtin_mul_overflow(b, d, &d1)) {
		throw std::runtime_error("MUL overflow.");
	}

	if (__builtin_sub_overflow(n1, n2, &n3)) {
		throw std::runtime_error("SUB overflow.");
	}


	return Rational{n3, d1};
}

Rational Rational::operator*(const Rational &other) const {
	int64_t a = this->num,
			b = this->den,
			c = other.numerator(),
			d = other.denominator();

	int64_t n1, d1;

	if (__builtin_mul_overflow(a, c, &n1) ||
		__builtin_mul_overflow(b, d, &d1)) {
		throw std::runtime_error("MUL overflow.");
	}

	return Rational{n1, d1};
}

// a/b / (c/d)
Rational Rational::operator/(const Rational &other) const {
	if (other.is_zero()) {
		throw std::runtime_error("can't divide by zero.");
	}

	int64_t a = this->num,
			b = this->den,
			c = other.numerator(),
			d = other.denominator();

	int64_t n1, d1;

	if (__builtin_mul_overflow(a, d, &n1) ||
		__builtin_mul_overflow(b, c, &d1)) {
		throw std::runtime_error("MUL overflow.");
	}

	return Rational{n1, d1};
}

bool Rational::operator==(const Rational &other) const {
	int64_t a = this->num,
			b = this->den,
			c = other.numerator(),
			d = other.denominator();

	int64_t a1, a2;

	if (__builtin_mul_overflow(a, d, &a1) ||
		__builtin_mul_overflow(b, c, &a2)) {
		throw std::runtime_error("MUL overflow");
	}

	return a1 == a2;
}

bool Rational::operator!=(const Rational &other) const {
	int64_t a = this->num,
			b = this->den,
			c = other.numerator(),
			d = other.denominator();

	int64_t a1, a2;

	if (__builtin_mul_overflow(a, d, &a1) ||
		__builtin_mul_overflow(b, c, &a2)) {
		throw std::runtime_error("MUL overflow");
	}

	return a1 != a2;
}

// a/b < c/d
bool Rational::operator<(const Rational &other) const {
	int64_t a = this->num,
			b = this->den,
			c = other.numerator(),
			d = other.denominator();

	int64_t a1, a2;

	if (__builtin_mul_overflow(a, d, &a1) ||
		__builtin_mul_overflow(b, c, &a2)) {
		throw std::runtime_error("MUL overflow");
	}

	return a1 < a2;
}

// a/b >= c/d
bool Rational::operator<=(const Rational &other) const {
	int64_t a = this->num,
			b = this->den,
			c = other.numerator(),
			d = other.denominator();

	int64_t a1, a2;

	if (__builtin_mul_overflow(a, d, &a1) ||
		__builtin_mul_overflow(b, c, &a2)) {
		throw std::runtime_error("MUL overflow");
	}

	return a1 <= a2;
}

// a/b > c/d
bool Rational::operator>(const Rational &other) const {
	int64_t a = this->num,
			b = this->den,
			c = other.numerator(),
			d = other.denominator();

	int64_t a1, a2;

	if (__builtin_mul_overflow(a, d, &a1) ||
		__builtin_mul_overflow(b, c, &a2)) {
		throw std::runtime_error("MUL overflow");
	}

	return a1 > a2;
}

// a/b >= c/d
bool Rational::operator>=(const Rational &other) const {
	int64_t a = this->num,
			b = this->den,
			c = other.numerator(),
			d = other.denominator();

	int64_t a1, a2;

	if (__builtin_mul_overflow(a, d, &a1) ||
		__builtin_mul_overflow(b, c, &a2)) {
		throw std::runtime_error("MUL overflow");
	}

	return a1 >= a2;
}

double Rational::val() const {
	return (double)this->num / this->den;
}

int64_t Rational::to_int() const {
	return this->num / this->den;
}

bool Rational::is_int() const {
	return (this->den == 1);
}

bool Rational::is_zero() const {
	return (this->num == 0);
}

int64_t Rational::numerator() const {
	return this->num;
}

int64_t Rational::denominator() const {
	return this->den;
}

std::pair<int64_t, int64_t> Rational::pair() const {
	return std::make_pair(this->num, this->den);
}

std::ostream& operator<<(std::ostream& os, const Rational& r) {
    if (r.denominator() == 1) {
        os << r.numerator();
    } else {
        os << r.numerator() << "/" << r.denominator();
    }

    return os;
}
