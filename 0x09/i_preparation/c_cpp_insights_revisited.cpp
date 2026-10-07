// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - The tool of unit 0x07 again, for what it shows best: C++ Insights (cppinsights.io) turns every lambda into the
 *   class the compiler writes for it.
 * - The lambda becomes a class with an `operator()` - `const`, unless the lambda is `mutable`.
 * - Every capture becomes a data member: `[limit]` an `int`, `[&count]` an `int&`. A constructor fills them in.
 * - A lambda with `auto` parameters gets a template `operator()`.
 * - A lambda without captures gets one more member: a conversion to a function pointer.
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <cstddef>
#include <cstdlib>
#include <cbl/printing.hpp>

using std::cout, std::vector;


/* ---- Content ---- */

/* -- .Outside the unnamed namespace. --
 * The next functions are the ones to paste into C++ Insights, with the two `#include`s for `vector` and `algorithm`.
 * They use nothing from `cbl`, so they can be pasted as they are - see below.
 */

/* --- `sort_descending` --- A lambda without captures. */
void sort_descending(vector<int>& numbers) {
    std::sort(numbers.begin(), numbers.end(), [](const int a, const int b) { return a > b; });
}

/* --- `any_above` --- A lambda that captures a copy. */
bool any_above(const vector<int>& numbers, const int limit) {
    const auto above = [limit](const int n) { return n > limit; };
    return std::any_of(numbers.begin(), numbers.end(), above);
}

/* --- `count_positive` --- A lambda that captures a reference. */
int count_positive(const vector<int>& numbers) {
    int count{0};
    std::for_each(numbers.begin(), numbers.end(), [&count](const int n) {
        if (n > 0) {
            ++count;
        }
    });
    return count;
}

/* --- `numbered` --- A `mutable` lambda: it changes its own copy, and keeps it from call to call. */
vector<int> numbered(const std::size_t n) {
    vector<int> result(n);
    int next{100};
    std::generate(result.begin(), result.end(), [next]() mutable { return next++; });
    return result;
}

/* --- `double_both` --- A generic lambda: `auto` for the parameter, called with an `int` and a `double`. */
double double_both(const int i, const double d) {
    const auto twice = [](const auto x) { return x + x; };
    return twice(i) + twice(d);
}

namespace {

    /* --- `call_them` ---
     * Run them first - nothing surprising here. Then paste them, see below.
     */
    void call_them() {
        print_function_header();

        vector<int> numbers{5, -2, 8, -1, 9, 3};
        sort_descending(numbers);
        cout << " 1| first=" << numbers.front() << ", any above 8: " << any_above(numbers, 8)
             << ", positive: " << count_positive(numbers) << '\n';
        cout << " 2| numbered(3).back()=" << numbered(3).back() << ", double_both(2, 1.5)=" << double_both(2, 1.5)
             << '\n';
    }

}

/* --- In C++ Insights ---
 * Open cppinsights.io, paste the two `#include`s and the five functions from `sort_descending` to `double_both`, and
 * press the button. On the right, every lambda has become a class, with a name like `__lambda_33_47` - the line and the
 * column where the lambda starts. The name is made up for the display; the compiler has a name, too, but you cannot
 * write it (see the session).
 * - `any_above`: the class has a data member `int limit`, a constructor that takes `limit` and copies it into the
 *   member, and `bool operator()(const int n) const`. Where the lambda was written, an object of the class is created:
 *   `__lambda_..._26 above = __lambda_..._26{limit};`. And `above` is passed to `any_of` - an object, by value.
 * - `count_positive`: the member is `int & count` - a reference, see the session for what that is in memory.
 * - `numbered`: `mutable` removes the `const` from `operator()` - that is all it does. The member `next` is changed by
 *   every call.
 * - `double_both`: `operator()` is a template, and below it the two versions the program uses - one for `int`, one
 *   for `double` (see previous snippets: a template generates code per type).
 * - `sort_descending`: no data member at all - and one more member function: a conversion to a function pointer,
 *   `bool (*)(int, int)`, which returns the address of a static function `__invoke`. Only a lambda without captures
 *   has it - see the session for why.
 * - !![#lambda]
 */

/* --- Predict ---
 * Before the session, predict for each of the five lambdas: how many bytes does an object of its class take? You know
 * `sizeof` of an `int`, and of an address. What about the class of `sort_descending`, which has no data member?
 */

/* --- `main` --- */
int main() {
    call_them();

    return EXIT_SUCCESS;
}
