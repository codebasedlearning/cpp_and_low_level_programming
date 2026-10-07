// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - Build it once yourself, then trust the library: a small `std::function` for `double(double)`, in 60 lines.
 * - Type erasure: a template constructor knows the type of the callable - and stores what is needed later as two
 *   function pointers, so that the class itself does not have to know the type.
 * - The two function pointers, in a table per type: the vtable of unit 0x08, by hand.
 * - A small callable in a buffer inside the object, a large one on the heap: the small buffer of unit 0x06.
 * - The function pointers in the table are lambdas without captures - converted, as in the session.
 */

#include <iostream>
#include <array>
#include <cstddef>
#include <new>                              // for placement new
#include <type_traits>
#include <utility>
#include <cstdlib>
#include <cbl/printing.hpp>
#include <cbl/heap_watch.hpp>

using std::cout, std::array;


/* ---- Content ---- */

namespace {

    /* --- `small_function` ---
     * Holds any callable that takes a `double` and returns a `double`.
     * - `buffer_` - 16 bytes inside the object, for a small callable.
     * - `object_` - the address of the callable: in `buffer_`, or on the heap.
     * - `table_` - the address of a table with two function pointers for exactly this type of callable: `call`, and
     *   `destroy`.
     * 16 + 8 + 8 = 32 bytes, as `std::function` in libstdc++. Copy and move are deleted, to keep it short: a real one
     * needs a third entry in the table, `copy`, and a move must not take along a pointer into its own buffer (see
     * previous snippets: the small buffer).
     */
    class small_function {
    public:
        template <typename F>
        small_function(F f) {               // not `explicit` - a lambda converts, as to `std::function`
            if constexpr (fits<F>) {
                object_ = new (buffer_.data()) F(std::move(f));
            } else {
                object_ = new F(std::move(f));
            }
            table_ = &table_for<F>;
        }

        ~small_function() {
            table_->destroy(object_);
        }

        small_function(const small_function&) = delete;
        small_function& operator=(const small_function&) = delete;

        double operator()(const double x) const {
            return table_->call(object_, x);
        }

        bool on_the_heap() const {
            return object_ != static_cast<const void*>(buffer_.data());
        }

    private:
        /* -- .The table. --
         * One per type `F`: a `static constexpr` variable template, made by the compiler for every `F` the program
         * uses - like a vtable per class. The two entries are lambdas without captures, converted to function
         * pointers. Each knows its `F`, and turns the `void*` back into an `F*`.
         */
        struct table {
            double (*call)(void* object, double x);
            void (*destroy)(void* object);
        };

        template <typename F>
        static constexpr bool fits{sizeof(F) <= 16 && alignof(F) <= alignof(std::max_align_t)};

        template <typename F>
        static constexpr table table_for{
            [](void* object, const double x) { return (*static_cast<F*>(object))(x); },
            [](void* object) {
                if constexpr (fits<F>) {
                    static_cast<F*>(object)->~F();
                } else {
                    delete static_cast<F*>(object);
                }
            }
        };

        alignas(std::max_align_t) array<unsigned char, 16> buffer_{};
        void* object_{nullptr};
        const table* table_{nullptr};
    };

    /* --- `half` --- A plain function. */
    double half(const double x) {
        return x / 2.0;
    }

    /* --- `hold_different_callables` ---
     * A function pointer, a lambda with two `double`s - 16 bytes - and a lambda with an array of ten - 80 bytes. The
     * first two go into the buffer, the third onto the heap: one allocation, and one release at the `}`.
     */
    void hold_different_callables() {
        print_function_header();

        const double a{2.0};
        const double b{1.0};
        const array<double, 10> weights{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
        cout << " 1| sizeof(small_function)=" << sizeof(small_function) << '\n';

        heap_watch heap{};
        {
            const small_function f{half};
            const small_function g{[a, b](const double x) { return a * x + b; }};
            const small_function h{[weights](const double x) { return weights[9] * x; }};
            cout << " 2| f(4)=" << f(4.0) << ", g(4)=" << g(4.0) << ", h(4)=" << h(4.0) << '\n';
            cout << " 3| on the heap: f " << f.on_the_heap() << ", g " << g.on_the_heap() << ", h " << h.on_the_heap()
                 << ", allocations=" << heap.allocations() << '\n';
        }
        cout << " 4| releases=" << heap.releases() << '\n';
    }

}

/* --- What `std::function` adds ---
 * - A copy: a third function pointer that copies the callable - into the buffer of the new object, or into a new block.
 * - The empty state: `table_` is null, `operator()` checks it and throws `bad_function_call`.
 * - Any signature: `R(Args...)` instead of `double(double)` - a variadic template, which this course does not cover.
 * - libstdc++ puts only callables into its buffer that can be copied byte by byte; libc++ has 24 bytes of room.
 * Look at your library's `<functional>` - search for `_M_manager` (libstdc++) or `__value_func` (libc++). It is the
 * same idea, with more care.
 */

/* --- `main` --- */
int main() {
    hold_different_callables();

    return EXIT_SUCCESS;
}
