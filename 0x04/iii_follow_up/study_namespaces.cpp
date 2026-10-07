// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - A named namespace keeps names apart: `geo::point` and the names in `std::` live side by side.
 * - A namespace is open: it can be continued in another block, in another header, in the `.cpp` file.
 * - Nested namespaces (`course::geo`) and a shorter alias (`namespace g = ...`).
 * - A using-declaration brings one name, a using-directive all of them - and why `using namespace` does not belong in a
 *   header.
 * - Argument-dependent lookup: why `cout << p` finds an `operator<<` that lives in `geo`.
 */

#include <iostream>
#include <string>
#include <cmath>
#include <cstdlib>
#include <cbl/printing.hpp>

using std::cout, std::ostream, std::string, std::sqrt;


/* ---- Content ---- */

/* -- .Outside the unnamed namespace. --
 * The namespaces of this file are at file scope, as they would be in a header - see previous snippets for what `nm`
 * shows for them.
 */

/* --- `geo` ---
 * Everything between the braces has the prefix `geo::`. Inside, the short names are enough: `distance` uses `point`
 * without `geo::`.
 * - !![#namespace]
 */
namespace geo {

    struct point {
        double x;
        double y;
    };

    double distance(const point& a, const point& b) {
        const double dx{a.x - b.x};
        const double dy{a.y - b.y};
        return sqrt(dx * dx + dy * dy);
    }

    ostream& operator<<(ostream& os, const point& p) {
        return os << '(' << p.x << ", " << p.y << ')';
    }

}

/* --- `geo`, continued ---
 * A second block adds to the same namespace - `std` is spread over dozens of headers in the same way.
 * In a real project, the header declares `double length(...)` inside `namespace geo { ... }`, and the `.cpp` file
 * defines it, either inside `namespace geo { ... }` again or with its full name: `double geo::length(...) { ... }`.
 */
namespace geo {

    double length(const point& p) {
        return distance(p, point{0.0, 0.0});
    }

}

/* --- `course::geo` ---
 * A nested namespace, written in one line (C++17). `course::geo::point` and `geo::point` are two different types.
 */
namespace course::geo {

    struct point {
        int col;
        int row;
    };

}

/* --- `metric` and `imperial` --- Two namespaces with the same name inside, for the clash below. */
namespace metric {

    const string length{"meter"};

}

namespace imperial {

    const string length{"yard"};

}

namespace {

    /* --- `use_qualified_names` ---
     * The full name always works, and it is the house style for `std::` in this course: `std::cout`, or a
     * using-declaration for it, see below.
     */
    void use_qualified_names() {
        print_function_header();

        const geo::point a{0.0, 0.0};
        const geo::point b{3.0, 4.0};
        cout << " 1| distance=" << geo::distance(a, b) << ", length=" << geo::length(b) << '\n';

        const course::geo::point cell{2, 5};
        cout << " 2| cell col=" << cell.col << ", row=" << cell.row << '\n';
    }

    /* --- `use_an_alias` ---
     * A long namespace gets a short name, only for this block: `namespace fs = std::filesystem;` is the usual example.
     */
    void use_an_alias() {
        print_function_header();

        namespace cg = course::geo;
        const cg::point cell{7, 1};
        cout << " 1| cell col=" << cell.col << ", row=" << cell.row << '\n';
    }

    /* --- `compare_declaration_and_directive` ---
     * `using geo::distance;` - a using-declaration - makes one name visible here, the one you asked for. `using
     * namespace geo;` - a using-directive - makes all of them visible, the ones you know and the ones you do not.
     * Both only until the `}` of this function.
     * - !![#using]
     */
    void compare_declaration_and_directive() {
        print_function_header();

        {
            using geo::distance;
            const geo::point b{3.0, 4.0};
            cout << " 1| distance=" << distance(geo::point{0.0, 0.0}, b) << '\n';
        }
        {
            using namespace geo;
            const point b{6.0, 8.0};
            cout << " 2| length=" << length(b) << '\n';
        }
    }

    /* --- `try_a_name_clash` ---
     * Two directives, one name in both: the compiler cannot choose, and says so - `reference to 'length' is ambiguous`.
     * With two namespaces of your own, you see the problem at once. With `using namespace std;` you bring in thousands
     * of names, and every new standard adds more (C++17 added `std::size` and `std::data`, C++20 `std::ssize`) - code
     * that compiled yesterday may not compile with the next compiler.
     * In a header it is worse: a `using namespace` there is forced on every file that includes it, and nobody can take
     * it back. Hence the house rule - `using std::cout, ...;` in a `.cpp` file, full names in a header.
     */
    void try_a_name_clash() {
        print_function_header();

        using namespace metric;
        using namespace imperial;
        // cout << length << '\n';              // does not compile: ambiguous
        cout << " 1| metric=" << metric::length << ", imperial=" << imperial::length << '\n';
    }

    /* --- `use_argument_dependent_lookup` ---
     * `length(p)` without `geo::` - and it compiles, outside `geo`, without any `using`. For an unqualified call, the
     * compiler also looks in the namespaces of the argument types: `p` is a `geo::point`, so `geo` is searched.
     * That is how `cout << p` finds `geo::operator<<`, and `cout << s` for a `string` finds `std::operator<<` - in
     * `a << b`, there is no place for a `geo::`.
     * - !![#adl]
     */
    void use_argument_dependent_lookup() {
        print_function_header();

        const geo::point p{3.0, 4.0};
        cout << " 1| length(p)=" << length(p) << '\n';
        cout << " 2| p=" << p << '\n';
    }

}

/* --- `main` --- */
int main() {
    use_qualified_names();
    use_an_alias();
    compare_declaration_and_directive();
    try_a_name_clash();
    use_argument_dependent_lookup();

    return EXIT_SUCCESS;
}
