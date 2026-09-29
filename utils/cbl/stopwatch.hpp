// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

#ifndef CBL_STOPWATCH_HPP
#define CBL_STOPWATCH_HPP

#include <chrono>                           // steady_clock, duration

/*
 * Measures the time since it was created, or since the last `reset()`:
 *
 *      stopwatch watch{};
 *      ...                                 // the work to measure
 *      cout << watch.elapsed_ms() << " ms\n";
 *
 * 'steady_clock' never jumps (unlike the wall clock, which may be adjusted), so it is the right clock for measuring
 * durations.
 *
 * Keep in mind that the optimizer may remove work whose result is never used - so always use (e.g. print) what you
 * computed while measuring.
 */
class stopwatch {
public:
    void reset() { start_ = clock::now(); }

    [[nodiscard]] double elapsed_ms() const {
        return std::chrono::duration<double, std::milli>(clock::now() - start_).count();
    }

private:
    using clock = std::chrono::steady_clock;
    clock::time_point start_{clock::now()};
};

#endif // CBL_STOPWATCH_HPP
