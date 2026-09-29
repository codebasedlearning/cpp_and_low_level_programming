// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

// One solution for task 'Millbrook', see ../tasks.md.

#include <iostream>
#include <array>
#include <vector>
#include <span>
#include <stdexcept>
#include <cstdlib>

using std::cout, std::array, std::vector, std::span, std::invalid_argument;

// Extension: an empty span has no smallest element - `values[0]` would read outside. So: throw.
int smallest(const span<const int> values) {
    if (values.empty()) {
        throw invalid_argument{"smallest of nothing"};
    }
    int result{values[0]};
    for (const int x : values) {
        if (x < result) {
            result = x;
        }
    }
    return result;
}

int largest(const span<const int> values) {
    if (values.empty()) {
        throw invalid_argument{"largest of nothing"};
    }
    int result{values[0]};
    for (const int x : values) {
        if (x > result) {
            result = x;
        }
    }
    return result;
}

long long sum(const span<const int> values) {
    long long result{0};
    for (const int x : values) {
        result += x;
    }
    return result;
}

// `span<int>`: changes the elements it looks at, wherever they are.
void clamp_all(const span<int> values, const int low, const int high) {
    for (int& x : values) {
        if (x < low) {
            x = low;
        } else if (x > high) {
            x = high;
        }
    }
}

void print(const span<const int> values) {
    for (const int x : values) {
        cout << ' ' << x;
    }
    cout << '\n';
}

int main() {
    const array<int, 5> a{4, -2, 9, 0, 7};
    vector<int> v{15, 3, -8, 42, 23, -1};

    cout << "a: smallest=" << smallest(a) << ", largest=" << largest(a) << ", sum=" << sum(a)
         << " (expected -2, 9, 18)\n";
    cout << "v: smallest=" << smallest(v) << ", largest=" << largest(v) << ", sum=" << sum(v)
         << " (expected -8, 42, 74)\n";

    const span<const int> middle{span<const int>{v}.subspan(1, 3)};     // 3, -8, 42
    cout << "middle: smallest=" << smallest(middle) << ", largest=" << largest(middle) << ", sum=" << sum(middle)
         << " (expected -8, 42, 37)\n";

    const size_t half{v.size() / 2};
    clamp_all(span<int>{v}.subspan(half), 0, 20);
    cout << "v after clamping the second half to [0, 20]:";
    print(v);
    cout << "(expected 15 3 -8 20 20 0)\n";

    try {
        cout << smallest(span<const int>{}) << '\n';
    } catch (const invalid_argument& e) {
        cout << "empty: " << e.what() << " (expected)\n";
    }

    // Extension: a pointer and a size - 16 bytes. With the size in the type, only the pointer is left - 8 bytes.
    cout << "sizeof(span<const int>)=" << sizeof(span<const int>)
         << ", sizeof(span<const int, 5>)=" << sizeof(span<const int, 5>) << '\n';

    return EXIT_SUCCESS;
}
