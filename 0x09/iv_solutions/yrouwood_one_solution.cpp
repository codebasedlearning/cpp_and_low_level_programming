// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

// One solution for task 'Yrouwood', see ../tasks.md. Measure as Release.

#include <iostream>
#include <cmath>
#include <functional>
#include <cstdlib>
#include <cbl/stopwatch.hpp>                // in your project: a copy of `stopwatch.hpp`

using std::cout, std::function;

template <typename F>
double approx_template(const double x0, const F& f, const double eps) {
    double x_old{x0};
    double x_new{f(x_old)};
    while (std::abs(x_new - x_old) > eps) {
        x_old = x_new;
        x_new = f(x_old);
    }
    return x_new;
}

double approx_pointer(const double x0, double (*const f)(double), const double eps) {
    double x_old{x0};
    double x_new{f(x_old)};
    while (std::abs(x_new - x_old) > eps) {
        x_old = x_new;
        x_new = f(x_old);
    }
    return x_new;
}

double approx_function(const double x0, const function<double(double)>& f, const double eps) {
    double x_old{x0};
    double x_new{f(x_old)};
    while (std::abs(x_new - x_old) > eps) {
        x_old = x_new;
        x_new = f(x_old);
    }
    return x_new;
}

// The function pointer cannot carry `a` - a function pointer is only the address of code. So `a` goes where every
// function can see it: into a global variable. That is what we give up - `heron_global` works for one `a` at a time,
// and not in two threads at once.
double global_a{2.0};

double heron_global(const double x) {
    return 0.5 * (x + global_a / x);
}

int main() {
    constexpr double eps{1e-10};

    for (const double a : {2.0, 4.0}) {
        const auto heron = [a](const double x) { return 0.5 * (x + a / x); };
        global_a = a;
        cout << "sqrt(" << a << "): template " << approx_template(1.0, heron, eps) << ", std::function "
             << approx_function(1.0, heron, eps) << ", pointer " << approx_pointer(1.0, heron_global, eps) << '\n';
        // approx_pointer(1.0, heron, eps);  // compiler error: a lambda with a capture is no function pointer
    }

    // Extension: a million square roots with each version.
    constexpr int n{1'000'000};
    stopwatch watch{};
    double sum_template{0.0};
    for (int i{1}; i <= n; ++i) {
        const double a{static_cast<double>(i)};
        sum_template += approx_template(1.0, [a](const double x) { return 0.5 * (x + a / x); }, eps);
    }
    const double template_ms{watch.elapsed_ms()};

    watch.reset();
    double sum_function{0.0};
    for (int i{1}; i <= n; ++i) {
        const double a{static_cast<double>(i)};
        sum_function += approx_function(1.0, [a](const double x) { return 0.5 * (x + a / x); }, eps);
    }
    const double function_ms{watch.elapsed_ms()};

    watch.reset();
    double sum_pointer{0.0};
    for (int i{1}; i <= n; ++i) {
        global_a = static_cast<double>(i);
        sum_pointer += approx_pointer(1.0, heron_global, eps);
    }
    const double pointer_ms{watch.elapsed_ms()};

    cout << "template:      " << template_ms << " ms, sum " << sum_template << '\n';
    cout << "std::function: " << function_ms << " ms, sum " << sum_function << '\n';
    cout << "pointer:       " << pointer_ms << " ms, sum " << sum_pointer << '\n';

    // Our runs, Release, gcc 13 and clang 18 on x86-64 Linux: template about 50 ms, pointer about the same,
    // std::function 75 to 100 ms. The pointer is known here: the compiler inlines `approx_pointer` into `main`, sees
    // `heron_global`, and calls it directly - but it may just as well not (see the session). The `std::function` is
    // built for every call of `approx_function` - no allocation, the lambda is 8 bytes and fits into it - and every
    // step goes through its invoker. The difference is smaller than in the session: every step of Heron's method has a
    // division, which takes longer than a call. The more work in the function, the less it matters how it is called.

    return EXIT_SUCCESS;
}
