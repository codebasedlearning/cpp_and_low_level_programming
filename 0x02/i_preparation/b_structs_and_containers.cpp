// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - `struct`: several values of different types under one name.
 * - `std::array` and `std::vector`: many values of one type.
 * - The range-based `for` loop.
 * - How they look in memory is the topic of the session.
 */

#include <iostream>
#include <array>
#include <vector>
#include <cstdlib>
#include <cbl/printing.hpp>

using std::cout, std::array, std::vector;


/* ---- Content ---- */

namespace {

    /* --- `fraction` ---
     * A `struct` groups data - like a class with public fields and nothing else.
     * - !![#aggregate]
     */
    struct fraction {
        int num;
        int denom;
    };

    /* --- `use_structs` --- No `new` - the variable is the object. */
    void use_structs() {
        print_function_header();

        fraction f{1, 2};                   // the members in declaration order
        cout << " 1| f.num=" << f.num << ", f.denom=" << f.denom << '\n';

        f.num = 5;                          // member access with `.`
        f.denom = 8;
        cout << " 2| f.num=" << f.num << ", f.denom=" << f.denom << '\n';

        const fraction g{};                 // all members 0
        cout << " 3| g.num=" << g.num << ", g.denom=" << g.denom << '\n';

        // Members can be named (C++20) - in declaration order only.
        const fraction h{.num = 3, .denom = 4};
        cout << " 4| h.num=" << h.num << ", h.denom=" << h.denom << '\n';
        // const fraction e{.denom = 4, .num = 3};  // compiler error: not in declaration order

        // Without braces, the members are not initialized - as with `int v;`. Reading them is undefined behavior.
        fraction u;
        u.num = 1;                          // fine: written before it is read
        u.denom = 2;
        cout << " 5| u.num=" << u.num << ", u.denom=" << u.denom << '\n';
    }

    /* --- `use_arrays` ---
     * `std::array<T, N>` holds exactly `N` values of type `T`. `N` must be known at compile time - that is what
     * `constexpr` says.
     * - !![#constexpr]
     */
    void use_arrays() {
        print_function_header();

        constexpr int dim{3};
        array<int, dim> a{2, 3, 5};
        a[0] = 1;                           // index from 0 to size()-1, not checked
        cout << " 1| a[0]=" << a[0] << ", a.at(2)=" << a.at(2) << ", a.size()=" << a.size() << '\n';

        // The old C-style array looks like this. It works, but it does not know its own size - prefer `std::array`.
        int raw[dim]{2, 3, 5};
        cout << " 2| raw[1]=" << raw[1] << '\n';
    }

    /* --- `use_vectors` --- `std::vector<T>` is like `std::array`, but it can grow. */
    void use_vectors() {
        print_function_header();

        vector<int> v{1, 2};
        v.push_back(3);                     // append at the end
        cout << " 1| v[0]=" << v[0] << ", v.at(2)=" << v.at(2) << ", v.size()=" << v.size() << '\n';
    }

    /* --- `loop_over_containers` ---
     * "For each element x in the container" - no index needed.
     * - !![#range-based-for]
     */
    void loop_over_containers() {
        print_function_header();

        const vector<int> primes{2, 3, 5, 7, 11};
        int sum{0};
        for (int p : primes) {
            sum += p;
        }
        cout << " 1| sum of primes=" << sum << '\n';
    }

}

/* --- Teaser ---
 * `use_structs` passes nothing around yet. What do you think happens to a `fraction` - or to a `vector` with a
 * million elements - when you pass it to a function?
 */

/* --- `main` --- */
int main() {
    use_structs();
    use_arrays();
    use_vectors();
    loop_over_containers();

    return EXIT_SUCCESS;
}
