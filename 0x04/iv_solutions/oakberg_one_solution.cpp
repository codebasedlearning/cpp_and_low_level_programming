// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

// One solution for task 'Oakberg', see ../tasks.md.

#include <iostream>
#include <string>
#include <array>
#include <vector>
#include <list>
#include <algorithm>
#include <iterator>
#include <cstdlib>

using std::cout, std::string, std::array, std::vector, std::list;
using std::find, std::next;

// Extension: all steps once, for every container with iterators. Only `++it` is used to move - so it works for a
// `list`, too. `it + 1` would work for `array` and `vector` only.
template <typename T>
void work_on(const string& name, T& container) {
    cout << name << ":";
    for (const int x : container) {
        cout << ' ' << x;
    }
    cout << '\n';

    cout << name << " with iterators:";
    for (auto it{container.begin()}; it != container.end(); ++it) {
        cout << ' ' << *it;
    }
    cout << '\n';

    for (int& x : container) {
        x *= 2;
    }

    cout << name << " from 6 on:";
    auto it{find(container.begin(), container.end(), 6)};
    for (int i{0}; i < 3 && it != container.end(); ++i, ++it) {
        cout << ' ' << *it;
    }
    cout << " (expected 6 10 14)\n";

    // Extension: `vector` and `array` - 4 bytes apart, one `int` after the other. `list` - wherever the nodes are.
    cout << name << " addresses:";
    for (auto a{container.begin()}; a != container.end(); ++a) {
        cout << ' ' << &*a;
    }
    cout << '\n';
}

int main() {
    array<int, 5> a{2, 3, 5, 7, 11};
    vector<int> v{2, 3, 5, 7, 11};
    list<int> l{2, 3, 5, 7, 11};

    work_on("array", a);
    work_on("vector", v);
    work_on("list", l);

    // Extension: with a `list`, `*(it + 1)` does not compile - no random access. `next(it, 1)` walks there.
    const auto six{find(l.begin(), l.end(), 6)};
    cout << "after 6 in the list: " << *next(six, 1) << " (expected 10)\n";

    return EXIT_SUCCESS;
}
