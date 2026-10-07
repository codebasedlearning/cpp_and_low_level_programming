// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - A function can be passed to another function, by its name: `sort` takes the comparison as an argument.
 * - A lambda is a function without a name, written where it is used: `[](const int a, const int b) { return a > b; }`.
 * - A lambda can be stored in an `auto` variable and called like a function.
 * - The `[]` says what the lambda takes from its surroundings: `[limit]` a copy, `[&count]` the variable itself.
 * - `std::function<double(double)>` is a type for "anything that can be called with a `double` and returns a
 *   `double`" - a function, a lambda, with or without captures.
 */

#include <iostream>
#include <vector>
#include <algorithm>                        // for sort, count_if, find_if, for_each
#include <functional>                       // for function
#include <cstdlib>
#include <cbl/printing.hpp>

using std::cout, std::vector, std::function;


/* ---- Content ---- */

namespace {

    /* --- `print_all` --- Prints the numbers of a vector in one line. */
    void print_all(const vector<int>& numbers) {
        for (const int n : numbers) {
            cout << ' ' << n;
        }
        cout << '\n';
    }

    /* --- `descending` --- A comparison: `true` if `a` belongs in front of `b`. */
    bool descending(const int a, const int b) {
        return a > b;
    }

    /* --- `pass_a_function_by_name` ---
     * `std::sort` sorts ascending, with `<`. To sort another way, pass a third argument: a function that takes two
     * elements and says whether the first belongs in front of the second. `descending` is passed by its name, without
     * `()` - it is not called here, `sort` calls it, as often as it needs to compare.
     * `std::ranges::sort(numbers, descending)` is the same with the container instead of the two iterators.
     */
    void pass_a_function_by_name() {
        print_function_header();

        vector<int> numbers{5, 2, 8, 1, 9, 3};
        std::sort(numbers.begin(), numbers.end());
        cout << " 1| ascending: ";
        print_all(numbers);

        std::sort(numbers.begin(), numbers.end(), descending);
        cout << " 2| descending:";
        print_all(numbers);
    }

    /* --- `write_a_lambda` ---
     * A function that is needed in exactly one place does not need a name: write it right there, as a lambda. It has
     * three parts - `[]` (the capture, see below), the parameters in `()`, and the body in `{}`. The return type is
     * taken from the `return` statement; `-> bool` after the parameters says it explicitly.
     * - !![#lambda]
     */
    void write_a_lambda() {
        print_function_header();

        vector<int> numbers{5, 2, 8, 1, 9, 3};
        std::sort(numbers.begin(), numbers.end(), [](const int a, const int b) { return a > b; });
        cout << " 1| descending:";
        print_all(numbers);

        vector<int> prices{25, 12, 38, 41, 9, 53};
        std::sort(prices.begin(), prices.end(), [](const int a, const int b) -> bool { return a % 10 < b % 10; });
        cout << " 2| by the last digit:";
        print_all(prices);
    }

    /* --- `store_a_lambda` ---
     * A lambda is a value: it can be stored in a variable - its type has no name you could write, so the variable is
     * `auto`. Then it is called like a function, and it can be passed on, as often as you like.
     */
    void store_a_lambda() {
        print_function_header();

        const auto is_even = [](const int n) { return n % 2 == 0; };
        cout << " 1| is_even(4)=" << is_even(4) << ", is_even(7)=" << is_even(7) << '\n';

        const vector<int> numbers{5, 2, 8, 1, 9, 4};
        cout << " 2| even numbers: " << std::count_if(numbers.begin(), numbers.end(), is_even) << '\n';
    }

    /* --- `capture_by_value` ---
     * A lambda sees its parameters - and nothing else of the function around it, unless it says so. `[limit]` takes a
     * copy of `limit`, when the lambda is created. Changing `limit` afterwards does not change the copy.
     * - !![#capture]
     */
    void capture_by_value() {
        print_function_header();

        const vector<int> numbers{5, 12, 8, 21, 9, 30};
        int limit{10};
        const auto above = [limit](const int n) { return n > limit; };
        limit = 20;

        const auto found{std::find_if(numbers.begin(), numbers.end(), above)};
        cout << " 1| first number above the limit: " << *found << ", limit now " << limit << '\n';

        /* -- .Q&A -- !![Why 12, and not 21?](#a-901) */
    }

    /* --- `capture_by_reference` ---
     * `[&count]` takes the variable itself, not a copy: the lambda can change it, and it sees every change. Here, the
     * lambda counts, and `for_each` calls it for every element.
     */
    void capture_by_reference() {
        print_function_header();

        const vector<int> numbers{5, -2, 8, -1, 9, 3};
        int count{0};
        std::for_each(numbers.begin(), numbers.end(), [&count](const int n) {
            if (n > 0) {
                ++count;
            }
        });
        cout << " 1| positive numbers: " << count << '\n';
    }

    /* --- `square` --- An ordinary function, for `integrate`. */
    double square(const double x) {
        return x * x;
    }

    /* --- `integrate` ---
     * The area below `f` between `a` and `b`, with the trapezoidal rule and `n` steps. `f` is a `std::function`: it
     * takes a `double` and returns one - what exactly it is, `integrate` does not care.
     * - !![#std-function]
     */
    double integrate(const function<double(double)>& f, const double a, const double b, const int n) {
        const double h{(b - a) / n};
        double sum{(f(a) + f(b)) / 2.0};
        for (int i{1}; i < n; ++i) {
            sum += f(a + i * h);
        }
        return sum * h;
    }

    /* --- `use_std_function` ---
     * `integrate` takes a function by its name, a lambda, and a lambda with a capture - all of them are callable with
     * a `double`. A variable of type `std::function` can hold one of them, and later another one.
     */
    void use_std_function() {
        print_function_header();

        const double k{3.0};
        cout << " 1| x^2: " << integrate(square, 0.0, 1.0, 1000) << '\n';
        cout << " 2| x^3: " << integrate([](const double x) { return x * x * x; }, 0.0, 1.0, 1000) << '\n';
        cout << " 3| k*x: " << integrate([k](const double x) { return k * x; }, 0.0, 1.0, 1000) << '\n';

        function<double(double)> f{square};
        cout << " 4| f(2)=" << f(2.0);
        f = [k](const double x) { return k + x; };
        cout << ", now f(2)=" << f(2.0) << '\n';
    }

}

/* --- `main` --- */
int main() {
    pass_a_function_by_name();
    write_a_lambda();
    store_a_lambda();
    capture_by_value();
    capture_by_reference();
    use_std_function();

    return EXIT_SUCCESS;
}
