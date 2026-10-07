// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - SIMD - single instruction, multiple data: one instruction adds four `int`s at once, in a 128-bit register
 *   (`paddd` on x86-64, `add v0.4s` on ARM64). The compiler finds such loops by itself - vectorization.
 * - Which build does it: clang at `-O2`; gcc at `-O3`, and from version 12 on at `-O2` for simple loops like the one
 *   here. CMake's Release build uses `-O3` anyway.
 * - The same loop over `float`s is not vectorized, and not even fast: floating-point addition is not associative, so
 *   the compiler must add in the order of the source - each addition waits for the one before.
 * - Four partial sums, by hand: four independent chains, four times faster - and a slightly different result. That is
 *   exactly why the compiler may not do it for you.
 * - Measure as Release, and small enough to stay in the cache - otherwise memory is the limit, not the instructions.
 */

#include <iostream>
#include <vector>
#include <cstddef>
#include <cstdlib>
#include <cbl/printing.hpp>
#include <cbl/stopwatch.hpp>

using std::cout, std::vector, std::size_t;


/* ---- Content ---- */

namespace {

    constexpr size_t count{4096};           // 16 KiB of `int`s or `float`s - fits into the fastest cache
    constexpr int repetitions{100'000};

    /* --- `sum_ints`, `sum_floats` and `sum_floats_in_four` ---
     * The same loop twice, and the `float` loop rewritten with four partial sums. Look at them in Compiler Explorer,
     * x86-64 and ARM64, `-O2` and `-O3`, gcc and clang (see the preparation): packed instructions (`paddd`,
     * `add v0.4s`) for the `int`s, scalar ones (`addss`, `fadd s0`) for the plain `float` loop.
     */
    int sum_ints(const vector<int>& values) {
        int sum{0};
        for (const int v : values) {
            sum += v;
        }
        return sum;
    }

    float sum_floats(const vector<float>& values) {
        float sum{0.0f};
        for (const float v : values) {
            sum += v;
        }
        return sum;
    }

    float sum_floats_in_four(const vector<float>& values) {
        float s0{0.0f};
        float s1{0.0f};
        float s2{0.0f};
        float s3{0.0f};
        for (size_t i{0}; i + 4 <= values.size(); i += 4) {
            s0 += values[i];
            s1 += values[i + 1];
            s2 += values[i + 2];
            s3 += values[i + 3];
        }
        return (s0 + s1) + (s2 + s3);       // `count` is a multiple of 4 - no rest to add
    }

    /* --- `measure_the_sums` ---
     * Each sum `repetitions` times. One element changes before each call, so the compiler cannot compute the sum once
     * and reuse it - and every total is printed, so it cannot drop the loops (see the stopwatch).
     * Measured on Linux: with gcc 11 on ARM64, the `int` sum takes half the time at `-O3`, vectorized, as at `-O2`,
     * where it is not. The `float` sum is the slowest in every build (gcc 11 on ARM64, gcc 13 and clang 18 on x86-64),
     * the one in four about four times faster.
     */
    void measure_the_sums() {
        print_function_header();

        vector<int> ints(count, 1);
        vector<float> floats(count, 0.1f);

        stopwatch watch{};
        long long int_total{0};
        for (int r{0}; r < repetitions; ++r) {
            ints[static_cast<size_t>(r) % count] ^= 1;
            int_total += sum_ints(ints);
        }
        const double int_ms{watch.elapsed_ms()};

        watch.reset();
        double float_total{0.0};
        for (int r{0}; r < repetitions; ++r) {
            floats[static_cast<size_t>(r) % count] += 1.0f;
            float_total += sum_floats(floats);
        }
        const double float_ms{watch.elapsed_ms()};

        watch.reset();
        double four_total{0.0};
        for (int r{0}; r < repetitions; ++r) {
            floats[static_cast<size_t>(r) % count] -= 1.0f;
            four_total += sum_floats_in_four(floats);
        }
        const double four_ms{watch.elapsed_ms()};

        cout << " 1| int:           " << int_ms << " ms (total " << int_total << ")\n";
        cout << " 2| float:         " << float_ms << " ms (total " << float_total << ")\n";
        cout << " 3| float in four: " << four_ms << " ms (total " << four_total << ")\n";
    }

    /* --- `compare_the_results` ---
     * 4096 times 0.1 is 409.6. Neither sum is exact - 0.1 is rounded, and so is every partial sum (floating point in
     * memory: see future snippets) - and they are not even equal: the order of the additions changed the rounding.
     * `-ffast-math` allows the compiler to reorder floating-point operations like this, and so to vectorize them - at
     * the price of such differences, and of checks for NaN that may silently disappear. Not a switch to set lightly.
     */
    void compare_the_results() {
        print_function_header();

        const vector<float> floats(count, 0.1f);
        cout << " 1| in order: " << sum_floats(floats) << ", in four: " << sum_floats_in_four(floats) << '\n';
    }

}

/* --- Why the `float` loop is slow ---
 * Not for a lack of SIMD alone. Each `sum += v` needs the result of the previous one, and a floating-point addition
 * takes several cycles until its result is ready - the loop waits, addition after addition. An `int` addition is ready
 * after one cycle. Four partial sums are four chains that do not wait for each other; the processor works on them at
 * the same time, and clang even packs them into one SIMD register.
 */

/* --- `main` --- */
int main() {
    measure_the_sums();
    compare_the_results();

    return EXIT_SUCCESS;
}
