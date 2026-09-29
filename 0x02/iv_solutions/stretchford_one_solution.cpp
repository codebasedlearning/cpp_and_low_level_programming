// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

// One solution for task 'Stretchford', see ../tasks.md.

#include <iostream>
#include <vector>
#include <cstdlib>

using std::cout, std::vector;

int main() {
    vector<int> v{};
    size_t capacity{v.capacity()};
    size_t copies{0};

    for (int i{0}; i < 1000; ++i) {
        if (v.size() == v.capacity()) {
            copies += v.size();             // full: all elements move to a larger block
        }
        v.push_back(i);
        if (v.capacity() != capacity) {
            cout << "size=" << v.size() << ", capacity " << capacity << " -> " << v.capacity() << '\n';
            capacity = v.capacity();
        }
    }
    cout << "element copies in total: " << copies << " for " << v.size() << " elements\n";

    // gcc and clang double the capacity (1, 2, 4, ..., 1024), MSVC grows by 1.5.
    // Extension: with doubling, the copies add up to 1 + 2 + 4 + ... + 512 = 1023 - about as many as elements. With any
    // constant factor the total stays proportional to n, so one `push_back` costs a constant amount on average
    // ("amortized").

    return EXIT_SUCCESS;
}
