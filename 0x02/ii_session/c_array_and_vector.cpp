// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - `std::array`: the elements are the object - on the stack, one after the other.
 * - `std::vector`: a small object on the stack, the elements on the heap.
 * - `size` vs. `capacity`: why `push_back` sometimes moves all elements.
 * - References into a container can dangle.
 */

#include <iostream>
#include <array>
#include <vector>
#include <cstdlib>
#include <cbl/printing.hpp>

using std::cout, std::array, std::vector;


/* ---- Content ---- */

namespace {

    /* --- `show_array_in_memory` ---
     * `sizeof` of a `std::array` is exactly the size of its elements - there is nothing else in it. The elements follow
     * each other without gaps.
     * - !![#containers]
     */
    void show_array_in_memory() {
        print_function_header();

        const array<int, 4> a{10, 20, 30, 40};
        cout << " 1| sizeof(a)=" << sizeof(a) << " bytes, &a=" << &a << '\n';
        for (size_t i{0}; i < a.size(); ++i) {
            cout << " 2|   &a[" << i << "]=" << &a[i] << '\n';
        }
    }

    /* --- `show_vector_in_memory` ---
     * A `vector` object always has the same size, no matter how many elements it holds: it only knows where its
     * elements are, how many there are, and how many would fit. The elements themselves are on the heap; `data()` gives
     * their address.
     * - !![#stack-and-heap]
     */
    void show_vector_in_memory() {
        print_function_header();

        const vector<int> small{1, 2, 3};
        const vector<int> large(1'000'000, 1);
        cout << " 1| sizeof(small)=" << sizeof(small) << ", sizeof(large)=" << sizeof(large) << '\n';
        cout << " 2| &small=" << &small << ", elements at " << small.data() << '\n';
        cout << " 3| &large=" << &large << ", elements at " << large.data() << '\n';
    }

    /* --- `grow_a_vector` ---
     * `size` is the number of elements, `capacity` the number that fit into the memory already reserved. When it is
     * full, `push_back` reserves a larger block, copies all elements over and releases the old block.
     */
    void grow_a_vector() {
        print_function_header();

        vector<int> v{};
        const int* elements{v.data()};      // an address, as in 0x01 - more on this type later
        int allocations{0};
        for (int i{0}; i < 1000; ++i) {
            v.push_back(i);
            if (v.data() != elements) {     // the elements are somewhere else now
                ++allocations;
                elements = v.data();
                cout << " 1|   size=" << v.size() << ", capacity=" << v.capacity()
                     << ", elements at " << elements << '\n';
            }
        }
        cout << " 2| " << allocations << " allocations for 1000 elements\n";

        /* -- .`reserve`. --
         * If you know the size in advance, reserve it once - then the elements never move.
         */
        vector<int> w{};
        w.reserve(1000);
        const int* before{w.data()};
        for (int i{0}; i < 1000; ++i) {
            w.push_back(i);
        }
        cout << " 3| with reserve: capacity=" << w.capacity()
             << ", elements moved: " << (w.data() != before) << '\n';
    }

    /* --- `invalidate_references` ---
     * A reference to an element refers to its place on the heap. When the vector moves its elements, that place is
     * released - and the reference dangles.
     * - !![#dangling-pointer]
     */
    void invalidate_references() {
        print_function_header();

        vector<int> v{1, 2, 3};
        const int& first{v[0]};
        cout << " 1| first=" << first << ", &first=" << &first
             << ", v.data()=" << v.data() << ", capacity=" << v.capacity() << '\n';

        v.push_back(4);                     // the vector is full - its elements move
        cout << " 2| v.data()=" << v.data() << '\n';

        // cout << first;                   // undefined behavior: `first` refers to released memory

        /* -- .Q&A -- !![What does `first` refer to after the `push_back`?](#a-204) */
    }

}

/* --- `main` --- */
int main() {
    show_array_in_memory();
    show_vector_in_memory();
    grow_a_vector();
    invalidate_references();

    return EXIT_SUCCESS;
}
