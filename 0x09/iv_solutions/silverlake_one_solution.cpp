// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

// One solution for task 'Silver Lake', see ../tasks.md. Run it as Release.

#include <iostream>
#include <vector>
#include <algorithm>
#include <functional>
#include <random>
#include <cstdlib>
#include <cbl/stopwatch.hpp>                // in your project: a copy of `stopwatch.hpp`

using std::cout, std::vector, std::function;

int compare_ints(const void* a, const void* b) {
    const int x{*static_cast<const int*>(a)};
    const int y{*static_cast<const int*>(b)};
    return (x > y) - (x < y);
}

bool less_than(const int a, const int b) {
    return a < b;
}

int main() {
    vector<int> numbers(1'000'000);
    std::mt19937 generator{23};
    std::uniform_int_distribution<int> pick{0, 1'000'000'000};
    for (int& n : numbers) {
        n = pick(generator);
    }

    vector<int> v{numbers};
    stopwatch watch{};
    std::qsort(v.data(), v.size(), sizeof(int), compare_ints);
    const double qsort_ms{watch.elapsed_ms()};

    v = numbers;
    watch.reset();
    std::sort(v.begin(), v.end(), less_than);
    const double pointer_ms{watch.elapsed_ms()};

    v = numbers;
    watch.reset();
    std::sort(v.begin(), v.end(), [](const int a, const int b) { return a < b; });
    const double lambda_ms{watch.elapsed_ms()};

    v = numbers;
    const function<bool(int, int)> wrapped{[](const int a, const int b) { return a < b; }};
    watch.reset();
    std::sort(v.begin(), v.end(), wrapped);
    const double function_ms{watch.elapsed_ms()};

    v = numbers;
    watch.reset();
    std::sort(v.begin(), v.end());
    const double default_ms{watch.elapsed_ms()};

    cout << "qsort:               " << qsort_ms << " ms\n";
    cout << "sort, pointer:       " << pointer_ms << " ms\n";
    cout << "sort, lambda:        " << lambda_ms << " ms\n";
    cout << "sort, std::function: " << function_ms << " ms\n";
    cout << "sort, default:       " << default_ms << " ms\n";

    // The comparisons, counted by a lambda that captures the counter by reference - in a run of its own, the counting
    // costs time. For `qsort`, the counter must be global: a function pointer carries nothing.
    long long comparisons{0};
    v = numbers;
    std::sort(v.begin(), v.end(), [&comparisons](const int a, const int b) {
        ++comparisons;
        return a < b;
    });
    cout << "std::sort: " << comparisons << " comparisons\n";

    // Our runs, Release, x86-64 Linux, gcc 13 and clang 18, with both libraries: qsort about 135 ms, a pointer 95,
    // std::function 120, a lambda and the default about 72. The lambda and the default `std::less<>` are equally fast:
    // both are types whose `operator()` the compiler knows, and inlines into `sort`. The function pointer is the same
    // function for every comparison - and yet a call: `sort` is instantiated for the type `bool (*)(int, int)`, not for
    // `less_than`. `std::function` adds its detour. `qsort` calls through a pointer, too, with two `const void*` to
    // convert back - it is one function in the C library, for all types, and cannot be instantiated for `int`.
    // 24 million comparisons with libstdc++ for a million numbers, 22.5 million with libc++ - n log2 n is 20 million.
    // Each one is a call, for everybody but the lambda and the default.
    // Extension, `nm -C` as Debug: libstdc++ has one `std::__introsort_loop<...>` per comparison - for
    // `_Iter_comp_iter<bool (*)(int, int)>`, for the two lambdas given to `sort` (`main::{lambda(int, int)#1}`, and
    // `#3`, the one that counts - `#2` is inside the `std::function`, and `sort` never sees it), for
    // `_Iter_comp_iter<std::function<...>>`, and `_Iter_less_iter` for the default: five instantiations of the whole
    // sort. That is the price of a template: one copy of the code per type.

    return EXIT_SUCCESS;
}
