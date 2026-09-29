// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - The everyday operations of a `vector`: create, access, insert, erase, grow and shrink.
 * - `vector<int> a(5, 23)` and `vector<int> b{5, 23}` are two very different vectors.
 * - `push_back` copies an element in, `emplace_back` builds it in place.
 * - Inserting or erasing at the front moves all elements behind it - at the back it is cheap.
 * - `clear` removes the elements, not the memory; `shrink_to_fit` asks to give it back.
 */

#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include <cstdlib>
#include <cbl/printing.hpp>

using std::cout, std::string, std::vector, std::out_of_range;


/* ---- Content ---- */

namespace {

    /* --- `print_all` --- From the session: works for every container with printable elements. */
    template <typename T>
    void print_all(const T& container) {
        for (const auto& x : container) {
            cout << ' ' << x;
        }
        cout << '\n';
    }

    /* --- `create_vectors` ---
     * Braces mean "these are the elements" - for a `vector`, the constructor that takes a list wins. Parentheses call
     * the other constructors: a count and a value, or a range given by two iterators. This is the one place where the
     * course's rule "brace initialization" needs parentheses.
     * - !![#uniform-initialization]
     */
    void create_vectors() {
        print_function_header();

        const vector<int> a(5, 23);         // five times 23
        const vector<int> b{5, 23};         // two elements: 5 and 23
        const vector<int> c(a.begin() + 1, a.end() - 1);    // a copy of a part of `a`
        const vector<int> d(3);             // three `int`s, value-initialized: 0
        cout << " 1| a:"; print_all(a);
        cout << " 2| b:"; print_all(b);
        cout << " 3| c:"; print_all(c);
        cout << " 4| d:"; print_all(d);
    }

    /* --- `access_elements` ---
     * `[]` does not check the index - out of range is undefined behavior. `at` checks and throws. `front` and `back` on
     * an empty vector are undefined behavior, too.
     */
    void access_elements() {
        print_function_header();

        vector<int> v{95, 96, 97, 98, 99};
        v[0] = 90;
        cout << " 1| v[3]=" << v[3] << ", v.at(4)=" << v.at(4) << ", front=" << v.front() << ", back=" << v.back()
             << '\n';
        try {
            cout << v.at(5);
        } catch (const out_of_range& e) {
            cout << " 2| at(5): " << e.what() << '\n';
        }
    }

    /* --- `insert_and_erase` ---
     * At the back, adding and removing is cheap. `insert` and `erase` take an iterator to the position - and move
     * every element behind it by one place. At the front of a million elements, that is a million moves.
     * `emplace_back(3, 'x')` passes its arguments to the constructor of the element: the `string` "xxx" is built right
     * in the vector's memory, without a temporary.
     */
    void insert_and_erase() {
        print_function_header();

        vector<int> v{1, 2, 3};
        v.push_back(4);
        v.insert(v.begin(), 0);             // at the front - all others move
        v.insert(v.begin() + 3, 42);        // before the element with index 3
        cout << " 1| v:"; print_all(v);

        v.pop_back();                       // the last one
        v.erase(v.begin());                 // the first one - all others move back
        v.erase(v.begin() + 1, v.begin() + 3);      // a range: [1, 3)
        cout << " 2| v:"; print_all(v);

        vector<string> words{};
        words.push_back(string(3, 'o'));    // a temporary `string`, then moved in (see future snippets)
        words.emplace_back(3, 'x');         // built in place
        cout << " 3| words:"; print_all(words);
    }

    /* --- `grow_and_shrink` ---
     * `resize` changes the size: new elements get the given value, surplus ones are destroyed. `clear` destroys all
     * elements - but keeps the memory, the capacity stays. `shrink_to_fit` asks the vector to release what it does not
     * need; it is a request, which a library may ignore (libstdc++ and libc++ do not).
     */
    void grow_and_shrink() {
        print_function_header();

        vector<int> v{1, 2, 3};
        v.resize(6, 9);
        cout << " 1| size=" << v.size() << ", capacity=" << v.capacity() << ", v:"; print_all(v);
        v.resize(2);
        cout << " 2| size=" << v.size() << ", capacity=" << v.capacity() << ", v:"; print_all(v);
        v.clear();
        cout << " 3| after clear: size=" << v.size() << ", capacity=" << v.capacity() << '\n';
        v.shrink_to_fit();
        cout << " 4| after shrink_to_fit: size=" << v.size() << ", capacity=" << v.capacity() << '\n';
    }

}

/* --- `main` --- */
int main() {
    create_vectors();
    access_elements();
    insert_and_erase();
    grow_and_shrink();

    return EXIT_SUCCESS;
}
