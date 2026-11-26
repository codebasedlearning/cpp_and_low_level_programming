// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - The motivation: Newton's method finds the zero of any function - so the function is a parameter.
 * - A function is code, and code is in memory: every function has an address. A function pointer holds it - and a
 *   call through it is an indirect call: which code runs is decided at run time, by the value of a variable.
 * - `using unary = int (*)(int);` gives the type a name, `unary f{&twice};` declares one - written out, it is
 *   `int (*f)(int)`. A function name turns into its address by itself.
 * - An array of function pointers picks the function by a number - the idea behind the vtable (see future snippets).
 * - A function pointer as a parameter passes behavior: the caller decides what the callee does.
 * - A pointer to a member function needs an object to be called with: `(c.*f)()`, or `(p->*f)()` for a pointer.
 */

#include <iostream>
#include <vector>
#include <cstdlib>
#include <array>
#include <functional>
#include <cmath>
#include <cbl/printing.hpp>

using std::cout, std::vector, std::array;


/* ---- Content ---- */

namespace {

    /* --- Newton's method for finding roots  ---
     * https://de.wikipedia.org/wiki/Newtonverfahren
     * https://en.wikipedia.org/wiki/Newton%27s_method
     *
     * x_n+1 = x_n - f(x_n) / f'(x_n)
     *
     * Example: `sqrt(2)` is a solution of `x*x-2 = 0`, `f(x)=x*x-2`
     *
     * Idea: Newton knows the method, not the problem: f and its derivative f' are parameters
     *
     *  double newton(function f, function f', double x, const int steps) {
     *      for (int i=1; i <= steps; ++i) {
     *          x -= f(x) / f'(x);
     *      }
     *      return x;
     *  }
     *
     * Two functions f and f' needed - but what is the type of the parameter?
     * => function pointers (address of a function in memory = start of the function in memory)
     */

    /* --- .`f` and `df` --- A function with a zero at `sqrt(2)`, and its derivative. */
    double f(const double x) { return x * x - 2.0; }
    double df(const double x) { return 2.0 * x; }

    /* --- .`real_function` --- The type, with a name. Read it as: a pointer to a function, `double` in, `double` out. */
    using real_function = double (*)(double);

    double newton(const real_function f, const real_function df, double x, const int steps) {
        for (int i{1}; i <= steps; ++i) {
            x -= f(x) / df(x);
            cout << " a| - step " << i << ": x=" << x << '\n';
        }
        return x;
    }

    /* --- `solve_with_newton` ---
     * The motivation: many problems, one method. Without function pointers, `newton` would be written once per problem,
     * or with a `switch` over every problem it knows.
     * Look at the digits of `sqrt(2)`: 1, 3, 6, 12, then all 16 correct - near the zero, every step doubles them.
     * After five steps, the result is exactly `std::sqrt(2.0)`.
     */
    void solve_with_newton() {
        print_function_header();

        const std::streamsize old_precision{cout.precision(16)};

        cout << " 1| sqrt(2), starting at 1:\n";
        const double root_of_2{newton(f, df, 1.0, 5)};
        cout << " 2| newton=" << root_of_2 << ", std::sqrt=" << std::sqrt(2.0) << '\n';

        cout.precision(old_precision);
    }

    /* --- .`half`, `square` and `negate` --- Three functions of the same type: an `int` in, an `double` out. */
    double half(const int n) { return n * 0.5; }
    double square(const int n) { return n * n; }
    double negate(const int n) { return -n; }

    /* --- .`operation` --- The type, with a name. Read it as: a pointer to a function, `int` in, `double` out. */
    using operation = double (*)(int);
    // preview: std::function<double(int)>
    // old:     typedef double(*operation)(int);

    /* --- `use_a_function_pointer` ---
     * `operation f` is a variable that holds the address of a function taking an `int` and returning a `double`.
     * Without the `using`, the type is written around the name: `double (*f)(int)`, see the comment below. There, the
     * parentheses matter: without them, `double* f(int)` declares a function that returns an `double*`.
     * A function name turns into its address by itself, as an array name does: `square` and `&square` are the same.
     * `f(3)` calls whatever `f` points to right now - the same line, different code. A function pointer can be
     * compared, and it can be `nullptr`: calling that is undefined behavior, as for any null pointer.
     * - !![#std-function]
     */
    void use_a_function_pointer() {
        print_function_header();

        operation f{half};                  // same as f{&half}
        // double (*f)(int){&half};         // also the same - that's why we use a typedef or using
        cout << " 1| half:   f(3)=" << f(3) << '\n';
        f = square;                         // same as &square
        cout << " 2| square: f(3)=" << f(3) << '\n';

        // operation g{nullptr};
        // g(3);                            // undefined behavior: a call through a null pointer
    }

    /* --- `pick_from_a_table` ---
     * An array of function pointers: the index picks the function. No `if`, no `switch` - one indirect call, through
     * whatever address is in the slot. Keep that picture, a table of addresses and an index: it is how a `virtual`
     * function is called (see future snippets).
     */
    void pick_from_a_table() {
        print_function_header();

        const array<operation,3> table{half, square, negate};
        for (int i{0}; i < 3; ++i) {
            cout << ' ' << i + 1 << "| table[" << i << "](5)=" << table[i](5) << '\n';
        }
    }

    /* --- .`apply_to_all` --- Changes every value - how, the caller decides, with `f`. */
    void apply_to_all(vector<int>& values, const operation f) {
        for (int& value : values) {
            value = f(value);
        }
    }

    /* --- `pass_a_function` ---
     * `apply_to_all` knows how to walk through the values, not what to do with each one. The caller passes that in, as
     * a function pointer: behavior as an argument, a callback. C works this way - `qsort` takes the comparison as a
     * function pointer - and the C++ algorithms take it as a parameter, too (see future units).
     */
    void pass_a_function() {
        print_function_header();

        vector<int> values{1, 2, 3};
        apply_to_all(values, square);
        cout << " 1| squared: " << values[0] << ' ' << values[1] << ' ' << values[2] << '\n';
        apply_to_all(values, negate);
        cout << " 2| negated: " << values[0] << ' ' << values[1] << ' ' << values[2] << '\n';
    }

    /* --- .`counter` --- Two member functions of the same type: no parameter, nothing returned, not `const`. */
    class counter {
    public:
        void up() { ++count_; }
        void down() { --count_; }
        int count() const { return count_; }

    private:
        int count_{0};
    };

    /* --- .`action` --- A pointer to a member function of `counter`: no parameter, nothing returned. */
    using action = void (counter::*)();     // cf. using operation = double (*)(int);

    /* --- `use_a_member_function_pointer` ---
     * `&counter::up` is not the address of a plain function: `up` has a hidden parameter, `this` - it needs a counter
     * to work on. So its type names the class, `void (counter::*)()`, and a call names the object: `(c.*step)()`, or
     * `(p->*step)()` through a pointer to it. The parentheses around `c.*step` are needed - without them, `()` binds
     * first. The `&` is needed, too: a member function name does not turn into a pointer by itself.
     * A table works as before: `program` is a list of steps, and the same loop runs them on any counter.
     * A pointer to a member function is 16 bytes with gcc and clang, twice a plain one: it must work for `virtual`
     * functions as well, and then it holds no address at all (see future units).
     */
    void use_a_member_function_pointer() {
        print_function_header();

        counter c;
        cout << " 1| start with c.count: " << c.count() << '\n';

        action step{&counter::up};          // here we need the &, above it is optional
        (c.*step)();                        // idea is as before, we just add the object to the game
        (c.*step)();
        cout << " 2| after two steps up: " << c.count() << '\n';

        counter* p{&c};                     // nothing new, just a counter-pointer
        step = &counter::down;              // member function pointer to step
        (p->*step)();                       // call it
        cout << " 3| and one down: " << c.count() << '\n';

        //const array program{&counter::up, &counter::up, &counter::up, &counter::down};
        vector<action> program;
        // [...] configure the programm dynamically
        program.push_back(&counter::up);
        program.push_back(&counter::up);
        program.push_back(&counter::up);
        program.push_back(&counter::down);

        counter result;
        cout << " 4| start with result.count: " << result.count() << '\n';

        for (const action a : program) {
            (result.*a)();
        }
        cout << " 5| result after the program: " << result.count() << '\n';

        // void (*plain)(){&counter::up};   // compiler error: a member function is no plain function
        // const action a{counter::up};     // compiler error: no `&` - the name does not turn into a pointer
        // c.*step();                       // compiler error: `step()` first - and `step` is no function
        // action wrong{&counter::count};   // compiler error: `count` returns an `int` and is `const`

        // `operation` is a plain function pointer, it only ever holds an address, so it's 8 bytes.
        // A member function pointer like `action` is 16 bytes for every class, because its type must also work for
        // classes with virtual functions and several bases. For counter, half of it is unused.
        cout << " 6| sizeof(unary)=" << sizeof(operation) << ", sizeof(action)=" << sizeof(action) << '\n';
    }

}

/* --- `main` --- */
int main() {
    solve_with_newton();
    use_a_function_pointer();
    pick_from_a_table();
    pass_a_function();
    use_a_member_function_pointer();

    return EXIT_SUCCESS;
}
