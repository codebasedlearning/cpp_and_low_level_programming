// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - C++17 gave many algorithms an execution policy as a first argument: `std::execution::seq` - one thread, as
 *   always - and `std::execution::par` - the library may split the work among threads.
 * - `std::reduce` is `accumulate` that may add in any order - which is what makes it parallel. For `double`s, the
 *   order changes the last digits.
 * - Whether `par` really runs in parallel depends on the library: MSVC does; libstdc++ only with Intel's TBB installed
 *   and linked, otherwise it quietly runs one thread; libc++ has no execution policies unless experimental.
 * - The snippet asks the library first.
 */

#include <iostream>
#include <iomanip>                          // for setprecision
#include <vector>
#include <numeric>
#include <algorithm>
#include <random>
#include <version>
#include <cstdlib>
#include <cbl/printing.hpp>
#include <cbl/stopwatch.hpp>

#if defined(__cpp_lib_parallel_algorithm)
#include <execution>
#endif

using std::cout, std::vector;


/* ---- Content ---- */

namespace {

    /* --- `add_up_in_parallel` ---
     * Fifty million `double`s, added up with `seq` and with `par`. Run it as Release. With a parallel library, `par`
     * takes a fraction of the time - not the number of cores as a factor: adding up is limited by the memory more than
     * by the cores. Without one, both take the same time.
     */
    void add_up_in_parallel() {
        print_function_header();

#if defined(__cpp_lib_parallel_algorithm)
        const vector<double> numbers(50'000'000, 0.1);
        stopwatch watch{};
        const double sequential{std::reduce(std::execution::seq, numbers.begin(), numbers.end())};
        const double seq_ms{watch.elapsed_ms()};
        watch.reset();
        const double parallel{std::reduce(std::execution::par, numbers.begin(), numbers.end())};
        const double par_ms{watch.elapsed_ms()};
        cout << " 1| seq: " << seq_ms << " ms, sum " << std::setprecision(17) << sequential << std::setprecision(6)
             << '\n';
        cout << " 2| par: " << par_ms << " ms, sum " << std::setprecision(17) << parallel << std::setprecision(6)
             << '\n';
#else
        cout << " 1| no parallel algorithms in this library\n";
#endif
    }

    /* --- `sort_in_parallel` ---
     * Ten million random `int`s, sorted with `seq` and with `par` - sorting has more work per byte than adding up, and
     * gains more from more cores.
     */
    void sort_in_parallel() {
        print_function_header();

#if defined(__cpp_lib_parallel_algorithm)
        vector<int> numbers(10'000'000);
        std::mt19937 generator{23};
        std::uniform_int_distribution<int> pick{0, 1'000'000'000};
        for (int& n : numbers) {
            n = pick(generator);
        }
        vector<int> copy{numbers};

        stopwatch watch{};
        std::sort(std::execution::seq, numbers.begin(), numbers.end());
        const double seq_ms{watch.elapsed_ms()};
        watch.reset();
        std::sort(std::execution::par, copy.begin(), copy.end());
        const double par_ms{watch.elapsed_ms()};
        cout << " 1| seq: " << seq_ms << " ms, par: " << par_ms << " ms, same result: " << (numbers == copy) << '\n';
#else
        cout << " 1| no parallel algorithms in this library\n";
#endif
    }

}

/* --- Where `par` is parallel ---
 * - MSVC: yes, with its own thread pool.
 * - libstdc++ (gcc, and clang on Linux): only if Intel's oneTBB is installed and the program is linked with `-ltbb`
 *   (in CMake: `find_package(TBB)` and `target_link_libraries(... TBB::tbb)`). Without it, `<execution>` compiles, and
 *   `par` runs in one thread - our Linux machines had no TBB, and `seq` and `par` took the same time.
 * - libc++ (Apple clang, and clang with `-stdlib=libc++`): not in version 18, only with `-fexperimental-library` in
 *   newer versions. Then this snippet says so.
 * A `par` algorithm must not have data races in the functions you give it - a lambda that writes to a captured
 * variable breaks exactly the rule of the session. And it may start threads: for a thousand elements, `par` is slower.
 */

/* --- `main` --- */
int main() {
    add_up_in_parallel();
    sort_in_parallel();

    return EXIT_SUCCESS;
}
