// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

// One solution for task 'Pleim', see ../tasks.md.

#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <functional>
#include <iterator>
#include <cstdlib>

using std::cout, std::vector;

int main() {
    // "n choose k" from the one before: C(n, k) = C(n, k - 1) * (n - k + 1) / k - exact in integers, because the
    // product is always divisible by k. The lambda keeps `k` and the last value in its members, from call to call.
    constexpr int n{4};
    vector<long long> row(n + 1);
    std::ranges::generate(row, [k = 0, value = 1LL]() mutable {
        if (k > 0) {
            value = value * (n - k + 1) / k;
        }
        ++k;
        return value;
    });
    cout << "n=" << n << ':';
    std::ranges::for_each(row, [](const long long c) { cout << ' ' << c; });
    cout << '\n';

    // Extension: Pascal's triangle. The inner numbers of a row are the sums of neighbors in the row before: `transform`
    // with two ranges - the row without its last number, and the row without its first - and `std::plus<>`. A 1 in
    // front and a 1 at the end. One loop, for the rows.
    vector<long long> current{1};
    for (int r{0}; r <= 10; ++r) {
        cout << "row " << r << ':';
        std::ranges::for_each(current, [](const long long c) { cout << ' ' << c; });
        cout << '\n';

        vector<long long> next{1};
        std::transform(current.begin(), current.end() - 1, current.begin() + 1, std::back_inserter(next),
                       std::plus<>{});
        next.push_back(1);
        current = std::move(next);
    }

    return EXIT_SUCCESS;
}
