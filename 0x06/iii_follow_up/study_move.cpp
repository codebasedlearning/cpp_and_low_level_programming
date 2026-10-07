// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - An lvalue has a name and lives on; an rvalue is about to die - a temporary, or what `std::move` hands over.
 * - A named `T&&` parameter is an lvalue again: to pass it on as an rvalue, `std::move` it once more.
 * - `std::move` is a cast to `T&&` - no machine code. The move itself happens in the constructor or the assignment that
 *   takes the `&&`.
 * - What is left of a moved-from `string`, `vector` and `unique_ptr` - guaranteed, and in practice.
 * - A `const` object cannot be moved from: `std::move` of it silently copies.
 * - A declared destructor switches off the generated moves: they silently become copies. `= default` brings them back.
 * - Returning a local: built in the caller's place, or moved. `return std::move(local)` prevents the first.
 * - Copy-and-swap: one assignment operator for copy and move, safe for self-assignment, and all or nothing if the copy
 *   fails.
 */

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <utility>
#include <cstddef>
#include <cstdlib>
#include <cbl/printing.hpp>
#include <cbl/heap_watch.hpp>

using std::cout, std::string, std::vector, std::unique_ptr, std::make_unique, std::size_t;


/* ---- Content ---- */

namespace {

    /* --- `kind` --- Two overloads: which one is chosen tells the category of the argument. */
    void kind(const string& s) {
        cout << " a|   lvalue: " << s << '\n';
    }

    void kind(string&& s) {
        cout << " b|   rvalue: " << s << '\n';
    }

    /* --- `pass_on` ---
     * `s` is a `string&&` - but it has a name, so inside the function it is an lvalue: it lives until the `}`, and it
     * could be used twice. To pass it on as an rvalue, `std::move` it again - once, at its last use.
     */
    void pass_on(string&& s) {
        kind(s);
        kind(std::move(s));
    }

    /* --- `name_the_categories` ---
     * A variable is an lvalue. A temporary - a literal turned into a `string`, the result of `+` - is an rvalue.
     * `std::move(title)` is an rvalue, too, and `title` is unchanged: nothing was moved, only the category changed.
     * - !![#value-categories]
     */
    void name_the_categories() {
        print_function_header();

        string title{"Giant Steps"};
        kind(title);
        kind(string{"Naima"});
        kind(title + "!");
        kind(std::move(title));
        cout << " 1| title is still: " << title << '\n';
        pass_on(string{"Mr. P.C."});
    }

    /* --- `look_at_the_moved_from` ---
     * The standard says: a moved-from object of the library is "valid but unspecified" - destroy it, or assign to it,
     * and ask nothing else. For some types it says more:
     * - a `unique_ptr` is `nullptr` after a move, guaranteed;
     * - a `vector` is empty after its move constructor, guaranteed;
     * - a `string` is empty in practice - with every library we know - but not by guarantee.
     */
    void look_at_the_moved_from() {
        print_function_header();

        string text{"A Love Supreme, Part I: Acknowledgement"};
        const string moved_text{std::move(text)};
        vector<int> numbers{1, 2, 3};
        const vector<int> moved_numbers{std::move(numbers)};
        unique_ptr<int> owner{make_unique<int>(42)};
        const unique_ptr<int> moved_owner{std::move(owner)};
        cout << " 1| text.size()=" << text.size() << ", numbers.size()=" << numbers.size() << ", owner is empty: "
             << (owner == nullptr) << '\n';

        text = "Resolution";                // assigning to a moved-from object is fine
        cout << " 2| text=" << text << '\n';
    }

    /* --- `try_to_move_a_const` ---
     * `std::move(c)` is a `const string&&`. The move constructor takes a `string&&` - without `const`, since it must
     * change its source - so it does not fit. The copy constructor, `const string&`, does. The result: a copy, one
     * allocation, and not a word from the compiler.
     */
    void try_to_move_a_const() {
        print_function_header();

        const string c{"My Favorite Things, the long version"};
        const heap_watch heap{};
        const string d{std::move(c)};
        cout << " 1| allocations=" << heap.allocations() << ", c is still: " << c << '\n';
    }

    /* --- `logged_buffer` ---
     * Owns its elements through a `vector` - the Rule of Zero - and declares one special member: a destructor, for a
     * log line. That is enough to switch off the generated move constructor and move assignment. The copy operations
     * are still generated, so every "move" is a copy - an allocation, and all the elements.
     */
    class logged_buffer {
    public:
        explicit logged_buffer(const size_t size) : data_(size) {}
        ~logged_buffer() { cout << " c|   logged_buffer: destroyed, size " << data_.size() << '\n'; }

        size_t size() const { return data_.size(); }

    private:
        vector<int> data_;
    };

    /* --- `defaulted_buffer` ---
     * The same with all five: the destructor, and `= default` for the other four - the compiler's versions, asked for
     * explicitly. Declaring the moves alone would delete the copies, so they are declared, too.
     * - !![#default-delete]
     */
    class defaulted_buffer {
    public:
        explicit defaulted_buffer(const size_t size) : data_(size) {}
        ~defaulted_buffer() { cout << " d|   defaulted_buffer: destroyed, size " << data_.size() << '\n'; }

        defaulted_buffer(const defaulted_buffer&) = default;
        defaulted_buffer& operator=(const defaulted_buffer&) = default;
        defaulted_buffer(defaulted_buffer&&) = default;
        defaulted_buffer& operator=(defaulted_buffer&&) = default;

        size_t size() const { return data_.size(); }

    private:
        vector<int> data_;
    };

    /* --- `lose_the_moves` ---
     * The same line, `std::move(a)`, twice. For `logged_buffer` it is a copy: one allocation, and `a` keeps its
     * elements. For `defaulted_buffer` it is a move: no allocation, and `a` is empty. Nothing in the call shows the
     * difference - only `heap_watch`, or a stopwatch.
     */
    void lose_the_moves() {
        print_function_header();

        logged_buffer a{1000};
        heap_watch heap{};
        const logged_buffer b{std::move(a)};
        cout << " 1| logged_buffer:    allocations=" << heap.allocations() << ", a.size()=" << a.size() << '\n';

        defaulted_buffer c{1000};
        heap.reset();
        const defaulted_buffer d{std::move(c)};
        cout << " 2| defaulted_buffer: allocations=" << heap.allocations() << ", c.size()=" << c.size() << '\n';

        /* -- .Q&A -- !![Which special members switch off the generated moves?](#a-609) */
    }

    /* --- `swap_buffer` ---
     * Owns `size_` `int`s on the heap - the Rule of Five by hand, as in the session, but with one assignment operator
     * instead of two. It takes its argument by value: an lvalue argument is copied into `other` - the copy constructor,
     * one allocation - an rvalue is moved - no allocation. Then `*this` and `other` swap their members, and `other`
     * takes the old elements with it to its destructor, at the `}`.
     * - If the copy fails (`bad_alloc`), it fails before the operator even starts: `*this` is untouched - all or
     *   nothing, the strong exception guarantee.
     * - `a = a` needs no check: a copy of `a` is swapped in, the old block is freed. It costs an allocation, but self
     *   assignment is rare.
     * - `swap` exchanges two pointers and two sizes - four words, nothing allocated, nothing that can throw, hence
     *   `noexcept`.
     * - !![#copy-and-swap]
     */
    class swap_buffer {
    public:
        explicit swap_buffer(const size_t size) : size_{size}, data_{new int[size]{}} {}
        ~swap_buffer() { delete[] data_; }

        swap_buffer(const swap_buffer& other) : size_{other.size_}, data_{new int[other.size_]} {
            for (size_t i{0}; i < size_; ++i) {
                data_[i] = other.data_[i];
            }
        }

        swap_buffer(swap_buffer&& other) noexcept : size_{other.size_}, data_{other.data_} {
            other.size_ = 0;
            other.data_ = nullptr;
        }

        swap_buffer& operator=(swap_buffer other) noexcept {    // by value: the copy or the move happens here
            swap(other);
            return *this;
        }

        void swap(swap_buffer& other) noexcept {
            std::swap(size_, other.size_);
            std::swap(data_, other.data_);
        }

        size_t size() const { return size_; }

    private:
        size_t size_;
        int* data_;
    };

    /* --- `assign_by_copy_and_swap` ---
     * Three assignments, one operator: from an lvalue (a copy, one allocation), from an rvalue (a move, none), and
     * to itself (a copy, one allocation - and still correct).
     */
    void assign_by_copy_and_swap() {
        print_function_header();

        swap_buffer a{1000};
        swap_buffer b{10};
        swap_buffer c{20};
        heap_watch heap{};
        b = a;
        cout << " 1| b = a:            allocations=" << heap.allocations() << ", b.size()=" << b.size() << '\n';

        heap.reset();
        b = std::move(c);
        cout << " 2| b = std::move(c): allocations=" << heap.allocations() << ", b.size()=" << b.size()
             << ", c.size()=" << c.size() << '\n';

        heap.reset();
        swap_buffer& same{a};               // `a = a` directly draws a warning from clang
        a = same;
        cout << " 3| a = a:            allocations=" << heap.allocations() << ", a.size()=" << a.size() << '\n';
    }

    /* --- `tracer` --- Reports copies and moves. */
    class tracer {
    public:
        explicit tracer(const string& name) : name_{name} {}
        tracer(const tracer& other) : name_{other.name_} { cout << " e|   copied " << name_ << '\n'; }
        tracer(tracer&& other) noexcept : name_{std::move(other.name_)} { cout << " f|   moved " << name_ << '\n'; }

        const string& name() const { return name_; }

    private:
        string name_;
    };

    /* --- `make_named`, `make_moved` and `pass_through` ---
     * Three ways to return a local object.
     * - `make_named` returns it by name. The compiler builds `t` directly in the caller's place (see previous
     *   snippets) - no copy, no move. Not guaranteed, but gcc and clang do it even as Debug, MSVC as Release; and
     *   where it is not done, the `return` moves - a local is about to die anyway.
     * - `make_moved` returns `std::move(t)` - an expression, not the name of a local. That rules out building `t` in
     *   place: now there is a move. gcc and clang warn with `-Wall` (`-Wpessimizing-move`) - switched off here, on
     *   purpose.
     * - `pass_through` returns a parameter. A parameter is built by the caller, so it cannot be built in the place of
     *   the result - the `return` moves it, without `std::move`.
     * - !![#copy-elision]
     */
    tracer make_named(const string& name) {
        tracer t{name};
        return t;
    }

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpessimizing-move"
    tracer make_moved(const string& name) {
        tracer t{name};
        return std::move(t);
    }
#pragma GCC diagnostic pop

    tracer pass_through(tracer t) {
        return t;
    }

    /* --- `return_a_local` --- Watch the ` f|` lines - where do they come from? */
    void return_a_local() {
        print_function_header();

        cout << " 1| make_named\n";
        const tracer a{make_named("a")};
        cout << " 2| make_moved\n";
        const tracer b{make_moved("b")};
        cout << " 3| pass_through\n";
        const tracer c{pass_through(tracer{"c"})};
        cout << " 4| " << a.name() << b.name() << c.name() << '\n';
    }

}

/* --- `main` --- */
int main() {
    name_the_categories();
    look_at_the_moved_from();
    try_to_move_a_const();
    lose_the_moves();
    assign_by_copy_and_swap();
    return_a_local();

    return EXIT_SUCCESS;
}
