// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

// One solution for task 'Kreley', see ../tasks.md.

#include <iostream>
#include <array>
#include <string_view>
#include <functional>
#include <memory>
#include <cstdlib>

using std::cout, std::cin, std::array, std::string_view, std::function, std::unique_ptr, std::make_unique;

using quotient_function = double (*)(int, int);

double q(const int x, const int y) {
    return static_cast<double>(x) / y;      // one operand `double` - no integer division
}

using binary = double (*)(double, double);

double add(const double a, const double b) { return a + b; }
double subtract(const double a, const double b) { return a - b; }
double multiply(const double a, const double b) { return a * b; }
double divide(const double a, const double b) { return a / b; }

int global_value{1};

int main() {
    const quotient_function f{q};
    const function<double(int, int)> g{q};
    cout << "f(1, 2)=" << f(1, 2) << ", g(1, 2)=" << g(1, 2) << '\n';
    cout << "sizeof: function pointer " << sizeof(f) << ", std::function " << sizeof(g) << '\n';

    // `q` and `global_value` are close to each other: both come from the program file, the code and the global data
    // are loaded next to each other. The stack and the heap are far away.
    const int local{0};
    const unique_ptr<int> on_the_heap{make_unique<int>(0)};
    cout << "q: " << reinterpret_cast<const void*>(q) << ", global: " << &global_value << ", local: " << &local
         << ", heap: " << on_the_heap.get() << '\n';

    // Extension: the calculator. `find` gives the position of the operator in "+-*/", and that is the index into the
    // table - a jump table, made by hand.
    constexpr array<binary, 4> operations{add, subtract, multiply, divide};
    constexpr string_view symbols{"+-*/"};
    cout << "a calculation, e.g. 3 * 4: ";
    double a{0.0};
    double b{0.0};
    char op{'+'};
    if (cin >> a >> op >> b) {
        if (const auto i{symbols.find(op)}; i != string_view::npos) {
            cout << a << ' ' << op << ' ' << b << " = " << operations[i](a, b) << '\n';
        } else {
            cout << "unknown operator " << op << '\n';
        }
    }

    // Extension, Compiler Explorer: `operations[i](a, b)` loads the address from the table - `call [QWORD PTR
    // operations[0+rax*8]]` or a load and `call rax` - an indirect call. With a constant operator, `-O2` computes the
    // index at compile time, calls the function directly, and inlines it: `mulsd` and nothing else.

    return EXIT_SUCCESS;
}
