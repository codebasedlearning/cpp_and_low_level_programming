// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

// One solution for task 'Ashford Mill', see ../tasks.md.

#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include <cstdlib>
#include <cbl/stopwatch.hpp>                // in your project: a copy of `stopwatch.hpp`

using std::cout, std::string, std::vector, std::runtime_error;

class scope_timer {
public:
    explicit scope_timer(const string& label) : label_{label} {}

    // Not copyable: two timers for one scope make no sense, and each would print.
    scope_timer(const scope_timer&) = delete;
    scope_timer& operator=(const scope_timer&) = delete;

    ~scope_timer() {
        cout << label_ << ": " << watch_.elapsed_ms() << " ms\n";
    }

private:
    string label_;
    stopwatch watch_{};
};

long long work(const bool fail) {
    const scope_timer timer{fail ? "work with error" : "work without error"};
    const vector<int> v(10'000'000, 1);
    long long sum{0};
    for (const int x : v) {
        sum += x;
    }
    if (fail) {
        throw runtime_error{"failed"};
    }
    return sum;
}

int main() {
    const long long sum{work(false)};       // the timer prints when `work` returns
    cout << "sum=" << sum << '\n';
    try {
        const long long sum{work(true)};
        cout << "sum=" << sum << " - never printed\n";
    } catch (const runtime_error& e) {
        cout << "caught: " << e.what() << '\n';   // the timer has printed already, during unwinding
    }

    // Extension: reverse order of construction - inner, second, first.
    {
        const scope_timer first{"first"};
        const scope_timer second{"second"};
        {
            const scope_timer inner{"inner"};
        }
    }

    // const scope_timer copy{first};       // compiler error: use of deleted function

    return EXIT_SUCCESS;
}
