// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

// One solution for task 'Copper View', see ../tasks.md.

#include <iostream>
#include <array>
#include <vector>
#include <list>
#include <iterator>
#include <cstdlib>

using std::cout, std::array, std::vector, std::list, std::begin, std::end;

int sum(const int* first, const int* last) {
    int result{0};
    for (const int* p{first}; p != last; ++p) {
        result += *p;
    }
    return result;
}

// `last` is one past the end, so the last element is at `last - 1`. Two elements or more left: swap the outer two.
void reverse(int* first, int* last) {
    while (last - first > 1) {
        --last;
        const int t{*first};
        *first = *last;
        *last = t;
        ++first;
    }
}

void print(const int* first, const int* last) {
    for (const int* p{first}; p != last; ++p) {
        cout << ' ' << *p;
    }
    cout << '\n';
}

// Extension: `last` for "not found", as `std::find` does.
const int* find_first(const int* first, const int* last, const int value) {
    for (const int* p{first}; p != last; ++p) {
        if (*p == value) {
            return p;
        }
    }
    return last;
}

// Extension: the same loop for everything that has `!=`, `++` and `*` - pointers and all iterators.
template <typename It>
int sum_all(It first, It last) {
    int result{0};
    for (It it{first}; it != last; ++it) {
        result += *it;
    }
    return result;
}

// Extension: `last - first` needs random access, a `list` iterator has no `-`. This version needs only `!=`, `==`, `++`
// and `--` - a bidirectional iterator, which a `list` has.
template <typename It>
void reverse_all(It first, It last) {
    while (first != last) {
        --last;
        if (first == last) {
            break;
        }
        const auto t{*first};
        *first = *last;
        *last = t;
        ++first;
    }
}

int main() {
    int a[5]{1, 2, 3, 4, 5};
    cout << "C array: sum=" << sum(begin(a), end(a)) << " (expected 15), reversed:";
    reverse(begin(a), end(a));
    print(begin(a), end(a));
    cout << "  first at " << &a[0] << ", last at " << &a[4] << ", end(a) - begin(a)=" << end(a) - begin(a) << '\n';

    array<int, 4> arr{10, 20, 30, 40};
    int* const arr_first{arr.data()};
    int* const arr_last{arr.data() + arr.size()};
    cout << "array:   sum=" << sum(arr_first, arr_last) << " (expected 100), reversed:";
    reverse(arr_first, arr_last);
    print(arr_first, arr_last);

    vector<int> v{7, 8, 9};
    int* const v_first{v.data()};
    int* const v_last{v.data() + v.size()};
    cout << "vector:  sum=" << sum(v_first, v_last) << " (expected 24), reversed:";
    reverse(v_first, v_last);
    print(v_first, v_last);
    cout << "  first at " << v_first << ", last at " << v_last - 1 << ", distance=" << v_last - v_first << '\n';

    // Extension: find_first.
    if (const int* found{find_first(begin(a), end(a), 3)}; found != end(a)) {
        cout << "found 3 at index " << found - begin(a) << '\n';
    }
    if (find_first(begin(a), end(a), 6) == end(a)) {
        cout << "6 not found\n";
    }

    // Extension: templates, with pointers and with iterators.
    list<int> l{1, 2, 3, 4};
    cout << "sum_all: " << sum_all(begin(a), end(a)) << ", " << sum_all(v.begin(), v.end()) << ", "
         << sum_all(l.begin(), l.end()) << '\n';
    reverse_all(l.begin(), l.end());
    cout << "list reversed:";
    for (const int x : l) {
        cout << ' ' << x;
    }
    cout << '\n';

    return EXIT_SUCCESS;
}
