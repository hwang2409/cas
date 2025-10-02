# Computer Algebra System (CAS)

A minimal computer algebra system built on a directed acyclic graph (DAG) for symbolic computation.

## Current Status

✅ **Expression Parser & Evaluator**
- Infix to postfix conversion with proper precedence/associativity
- Unary minus handling (`-x`, `-(x+y)`)
- N-ary addition and multiplication
- Built-in functions: `sin`, `cos`, `tan`, `log`, `exp`, `sqrt`, `abs`
- Constants: `pi`, `e`, `tau`
- Variable substitution and evaluation

## Development Roadmap

### Phase 1: Core Infrastructure ✅
- [x] DAG-based expression representation
- [x] Parser with shunting-yard algorithm
- [x] Expression evaluation with variable substitution
- [x] Operator precedence and associativity
- [x] Unary operations and functions

### Phase 2: Pretty Printing & LaTeX
- [ ] `to_string()` with minimal parentheses
- [ ] `to_latex()` with proper mathematical formatting
- [ ] Function rendering (`sin`, `cos`, `sqrt`, `abs`)
- [ ] Fraction and power notation

**Implementation Details:**
- [ ] Implement `eDAG::to_string()` method
  - [ ] Recursive tree traversal with precedence-aware parentheses
  - [ ] Handle n-ary `+/*` with proper spacing
  - [ ] Unary minus rendering: `-x`, `-(x+y)`
  - [ ] Function calls: `sin(x)`, `sqrt(x)`, `abs(x)`
- [ ] Implement `eDAG::to_latex()` method
  - [ ] LaTeX operators: `\cdot`, `\frac{num}{den}`, `\sqrt{x}`
  - [ ] Powers: `{base}^{exp}`, absolute value: `\left|x\right|`
  - [ ] Functions: `\sin`, `\cos`, `\tan`, `\log`, `\exp`
- [ ] Add precedence/associativity logic for minimal parentheses
- [ ] Test cases: `2^3^2`, `a-b-c`, `-(x+y)`, `sin(cos(x))`

### Phase 3: Simplification Engine
- [ ] Basic algebraic identities
  - [ ] `x + 0 → x`, `0 + x → x`
  - [ ] `x * 1 → x`, `x * 0 → 0`
  - [ ] `x - 0 → x`, `0 - x → -x`
  - [ ] `x^1 → x`, `x^0 → 1` (x ≠ 0)
- [ ] Function simplifications
  - [ ] `sin(0) = 0`, `cos(0) = 1`
  - [ ] `log(1) = 0`, `exp(0) = 1`
  - [ ] `log(exp(x)) → x`
- [ ] Constant folding
- [ ] Associative flattening for `+/*`
- [ ] Commutative sorting

**Implementation Details:**
- [ ] Implement `eDAG::simplify()` method
  - [ ] Pattern matching system for rewrite rules
  - [ ] Constant folding: evaluate all-constant subtrees
  - [ ] Identity elimination: `x+0→x`, `x*1→x`, `x*0→0`
  - [ ] Function simplifications: `sin(0)→0`, `cos(0)→1`, `log(1)→0`
- [ ] Add rewrite rule engine
  - [ ] Rule structure: `pattern → replacement`
  - [ ] Tree matching and substitution
  - [ ] Iterative application until no more changes
- [ ] Implement canonicalization
  - [ ] Flatten associative operations: `(a+b)+c → a+b+c`
  - [ ] Sort commutative operands: `b+a → a+b`
  - [ ] Normalize signs: `-(-x) → x`
- [ ] Test cases: `x+0`, `x*1`, `sin(0)`, `log(exp(x))`, `2*x+3*x`

### Phase 4: Exact Arithmetic
- [ ] `Rational` class with GCD normalization
- [ ] Double to rational conversion (continued fractions)
- [ ] Exact arithmetic operations
- [ ] Mixed exact/approximate evaluation
- [ ] Overflow protection

**Implementation Details:**
- [ ] Create `Rational` class in `edag.hpp`
  - [ ] Constructor: `Rational(int64_t num, int64_t den = 1)`
  - [ ] Arithmetic operators: `+`, `-`, `*`, `/`, unary `-`
  - [ ] Comparison operators: `==`, `!=`, `<`, `<=`, `>`, `>=`
  - [ ] Conversion methods: `to_double()`, `to_string()`, `to_latex()`
  - [ ] Utility methods: `is_integer()`, `is_zero()`, `is_one()`
- [ ] Implement double→rational conversion
  - [ ] Continued fractions algorithm for best approximation
  - [ ] Stern-Brocot tree fallback for bounded denominators
  - [ ] Overflow protection with `int64_t` limits
- [ ] Modify `eNode` to support exact arithmetic
  - [ ] Add `Number` variant: `std::variant<int64_t, Rational, double>`
  - [ ] Update evaluation to use exact arithmetic when possible
  - [ ] Fall back to double precision when needed
- [ ] Test cases: `1/3 + 2/3 = 1`, `0.1 + 0.2 = 3/10`, overflow handling

### Phase 5: Polynomial Support
- [ ] Monomial representation
- [ ] Sparse polynomial storage
- [ ] Polynomial arithmetic (`+`, `-`, `*`)
- [ ] Like term combining
- [ ] Polynomial detection in DAG
- [ ] Degree calculation
- [ ] Evaluation and substitution

**Implementation Details:**
- [ ] Create `Monomial` class
  - [ ] Structure: `std::map<std::string, int> vars` (variable → exponent)
  - [ ] Methods: `degree()`, `is_constant()`, `multiply()`, `divide()`
  - [ ] Ordering: lexicographic for canonical form
- [ ] Create `Polynomial` class
  - [ ] Structure: `std::map<Monomial, Rational> terms` (sparse representation)
  - [ ] Arithmetic: `+`, `-`, `*` with like-term combining
  - [ ] Methods: `degree()`, `leading_coefficient()`, `evaluate()`
- [ ] Add polynomial detection to `eDAG`
  - [ ] Identify polynomial subexpressions in DAG
  - [ ] Convert eligible DAG nodes to polynomial form
  - [ ] Hybrid representation: polynomial parts + DAG parts
- [ ] Implement polynomial operations
  - [ ] Addition: combine like terms, `2*x + 3*x → 5*x`
  - [ ] Multiplication: distribute and combine
  - [ ] Evaluation: substitute values for variables
- [ ] Test cases: `x^2 + 2*x + 1`, `(x+1)*(x-1) = x^2-1`, `2*x + 3*x = 5*x`

### Phase 6: Symbolic Differentiation
- [ ] Basic differentiation rules
  - [ ] `d/dx(x) = 1`, `d/dx(c) = 0`
  - [ ] Linearity: `d/dx(af + bg) = a*f' + b*g'`
  - [ ] Product rule: `d/dx(fg) = f'g + fg'`
  - [ ] Quotient rule: `d/dx(f/g) = (f'g - fg')/g²`
  - [ ] Chain rule: `d/dx(f(g)) = f'(g) * g'`
- [ ] Function derivatives
  - [ ] `sin' → cos`, `cos' → -sin`
  - [ ] `tan' → sec²`, `log' → 1/x`
  - [ ] `exp' → exp`, `sqrt' → 1/(2√x)`
- [ ] Power rule: `d/dx(x^n) = n*x^(n-1)`
- [ ] Automatic simplification of derivatives

**Implementation Details:**
- [ ] Implement `eDAG::derivative(const std::string& var)` method
  - [ ] Recursive differentiation with rule application
  - [ ] Variable identification and constant detection
  - [ ] Rule-based transformation system
- [ ] Add differentiation rules
  - [ ] Basic rules: `d/dx(x) = 1`, `d/dx(c) = 0`
  - [ ] Linearity: `d/dx(a*f + b*g) = a*f' + b*g'`
  - [ ] Product rule: `d/dx(f*g) = f'*g + f*g'`
  - [ ] Quotient rule: `d/dx(f/g) = (f'*g - f*g')/g^2`
  - [ ] Chain rule: `d/dx(f(g)) = f'(g) * g'`
- [ ] Implement function derivatives
  - [ ] Trig functions: `sin' → cos`, `cos' → -sin`, `tan' → sec^2`
  - [ ] Log/exp: `log' → 1/x`, `exp' → exp`
  - [ ] Power functions: `sqrt' → 1/(2*sqrt(x))`, `abs' → sign(x)`
- [ ] Add power rule handling
  - [ ] `d/dx(x^n) = n*x^(n-1)` for constant n
  - [ ] `d/dx(a^x) = a^x*ln(a)` for constant a
  - [ ] General case via chain rule: `d/dx(x^y) = x^y*(y/x + y'*ln(x))`
- [ ] Test cases: `d/dx(x^2) = 2*x`, `d/dx(sin(x)) = cos(x)`, `d/dx(x*sin(x))`

### Phase 7: Advanced Features
- [ ] Substitution system
- [ ] Canonicalization
- [ ] GCD and factorization
- [ ] Univariate polynomial solving
- [ ] Complex number support
- [ ] Matrix operations
- [ ] Series expansion

**Implementation Details:**
- [ ] Implement `eDAG::substitute()` method
  - [ ] Variable replacement with expression substitution
  - [ ] Safe substitution avoiding variable capture
  - [ ] Re-simplification after substitution
- [ ] Add canonicalization system
  - [ ] Standard form for expressions
  - [ ] Deterministic ordering of terms
  - [ ] Normalized representation
- [ ] Implement polynomial GCD
  - [ ] Euclidean algorithm for univariate polynomials
  - [ ] Extended GCD for coefficient computation
  - [ ] Factorization over rationals
- [ ] Add polynomial solving
  - [ ] Linear equations: `ax + b = 0`
  - [ ] Quadratic formula: `ax² + bx + c = 0`
  - [ ] Cubic/quartic formulas (optional)
  - [ ] Numerical fallback for higher degrees
- [ ] Test cases: `substitute(x, y+1, x^2) = (y+1)^2`, `gcd(x^2-1, x-1) = x-1`

### Phase 8: User Interface
- [ ] REPL/CLI interface
- [ ] Command parsing
- [ ] Error handling and diagnostics
- [ ] Interactive help system
- [ ] Batch processing

**Implementation Details:**
- [ ] Create CLI interface in `main.cpp`
  - [ ] Command loop with prompt: `cas> `
  - [ ] Parse commands: `parse`, `print`, `latex`, `simplify`, `diff`, `eval`
  - [ ] Variable assignment: `x = 2`, `y = x + 1`
- [ ] Add command parsing system
  - [ ] Command structure: `command [args]`
  - [ ] Expression parsing for arguments
  - [ ] Variable substitution in expressions
- [ ] Implement error handling
  - [ ] Syntax error reporting with line numbers
  - [ ] Runtime error messages with context
  - [ ] Graceful error recovery
- [ ] Add help system
  - [ ] Command documentation
  - [ ] Example usage for each command
  - [ ] Built-in tutorial mode
- [ ] Test cases: `parse "x^2 + 1"`, `diff x "x^2"`, `eval x=2 "x^2"`

## Example Usage

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
	// Output: 390625

	return 0;
}
```

```C++
#include "edag.hpp"
#include <string>

int main() {
	eDAG t1, t2;

	t1.parse("cos(theta)");
	t2.parse("(2 + 3) / 5 * PI");

	double theta = t2.eval();
	std::cout << t1.eval({{"theta", theta}}) << "\n";
	// Output: -1

	return 0;
}
```

## Testing Strategy

### Core Functionality Tests
- [ ] Parsing precedence: `2^3^2`, `a-b-c`, `-x`, `-(x+y)`
- [ ] Evaluation accuracy with various inputs
- [ ] Error handling for invalid expressions
- [ ] Round-trip: `parse → to_string → parse`

### Simplification Tests
- [ ] Identity rules: `x+0→x`, `x*1→x`, `x*0→0`
- [ ] Function simplifications: `sin(0)→0`, `log(exp(x))→x`
- [ ] Like term combining: `2*x+3*x→5*x`

### Differentiation Tests
- [ ] Basic rules: `d/dx(x^n)`, `d/dx(sin(x))`, `d/dx(log(x))`
- [ ] Chain rule: `d/dx(sin(x^2))`
- [ ] Product rule: `d/dx(x*sin(x))`
- [ ] Numerical validation against finite differences

### Performance Tests
- [ ] Large expression parsing
- [ ] Deep nesting evaluation
- [ ] Memory usage optimization
- [ ] Evaluation speed benchmarks
