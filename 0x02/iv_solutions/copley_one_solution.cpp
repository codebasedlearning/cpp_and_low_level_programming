// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

// One solution for task 'Copley', see ../tasks.md.

#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <cbl/stopwatch.hpp>                // copied into your own project

using std::cout, std::string, std::vector;

void measure(const vector<string>& v) {
    stopwatch watch{};
    size_t total{0};
    for (auto s : v) {                      // a copy of every string
        total += s.size();
    }
    cout << "  auto:        total=" << total << ", " << watch.elapsed_ms() << " ms\n";

    watch.reset();
    total = 0;
    for (const auto& s : v) {               // no copy
        total += s.size();
    }
    cout << "  const auto&: total=" << total << ", " << watch.elapsed_ms() << " ms\n";
}

int main() {
    cout << "1'000'000 strings with 40 characters:\n";
    measure(vector<string>(1'000'000, string(40, 'x')));

    cout << "1'000'000 strings with 10 characters:\n";
    measure(vector<string>(1'000'000, string(10, 'x')));

    // With 40 characters, every copy needs its own heap block: allocate, copy, release - a million times. With 10
    // characters, the text fits into the string object itself (small string optimization, 0x01), so a copy is just 32
    // bytes - no heap at all.

    return EXIT_SUCCESS;
}
