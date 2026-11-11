// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - Lvalues, rvalues, and `std::move` as a cast are known from the session (see previous snippets). One more rule: a
 *   named `T&&` parameter is an lvalue again - to pass it on as an rvalue, `std::move` it once more.
 * - What is left of a moved-from `string`, `vector` and `unique_ptr` - guaranteed, and in practice.
 * - A `const` object cannot be moved from: `std::move` of it silently copies.
 * - A declared destructor switches off the generated moves: they silently become copies. `= default` brings them back.
 * - Moving a `vector` copies three words, whatever its size.
 * - A growing `vector` moves its elements only if their move constructor is `noexcept` - otherwise it copies them.
 */

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <utility>
#include <cstddef>
#include <cstdlib>
#include <cbl/printing.hpp>
#include <cbl/stopwatch.hpp>

using std::cout, std::string, std::vector, std::unique_ptr, std::make_unique, std::size_t;


/* ---- Content ---- */

namespace {

    /* --- .`kind` --- Two overloads: which one is chosen tells the category of the argument (see previous snippets). */
    void kind(const string& s) {
        cout << " a| - lvalue: " << s << '\n';
    }

    void kind(string&& s) {
        cout << " b| - rvalue: " << s << '\n';
    }

    /* --- .`pass_on` ---
     * `s` is a `string&&` - but it has a name, so inside the function it is an lvalue: it lives until the `}`, and it
     * could be used twice. To pass it on as an rvalue, `std::move` it again - once, at its last use.
     */
    void pass_on(string&& s) {
        kind(s);
        kind(std::move(s));
    }

    /* --- `pass_an_rvalue_on` ---
     * `pass_on` gets a temporary - an rvalue. Inside, the first `kind` says lvalue: the parameter has a name. Only
     * `std::move(s)` makes it an rvalue again.
     * - !![#value-categories]
     */
    void pass_an_rvalue_on() {
        print_function_header();

        pass_on(string{"Hi there."});
    }

    /* --- `try_to_move_a_const` ---
     * `std::move(c)` is a `const string&&`. The move constructor takes a `string&&` - without `const`, since it must
     * change its source - so it does not fit. The copy constructor, `const string&`, does. The result: a copy, one
     * allocation, and not a word from the compiler.
     */
    void try_to_move_a_const() {
        print_function_header();

        const string s{"My Favorite Things, the long version"};
        const string t{std::move(s)};

        cout << " 1| s.data=" << static_cast<const void*>(s.data())
             << ", t.data=" << static_cast<const void*>(t.data()) << '\n';
    }

    /* --- .`logged_buffer` ---
     * Owns its elements through a `vector` - the Rule of Zero - and declares one special member: a destructor, for a
     * log line. That is enough to switch off the generated move constructor and move assignment. The copy operations
     * are still generated, so every "move" is a copy - an allocation, and all the elements.
     */
    class logged_buffer {
    public:
        explicit logged_buffer(const size_t size) : data_(size) {}
        ~logged_buffer() { cout << " a| - logged_buffer destroyed, size " << data_.size() << '\n'; }

        size_t size() const { return data_.size(); }
        const int* data() const { return data_.data(); }

    private:
        vector<int> data_;
    };

    /* --- .`defaulted_buffer` ---
     * The same with all five: the destructor, and `= default` for the other four - the compiler's versions, asked for
     * explicitly. Declaring the moves alone would delete the copies, so they are declared, too.
     * - !![#default-delete]
     */
    class defaulted_buffer {
    public:
        explicit defaulted_buffer(const size_t size) : data_(size) {}
        ~defaulted_buffer() { cout << " b| - defaulted_buffer destroyed, size " << data_.size() << '\n'; }

        defaulted_buffer(const defaulted_buffer&) = default;
        defaulted_buffer& operator=(const defaulted_buffer&) = default;
        defaulted_buffer(defaulted_buffer&&) = default;
        defaulted_buffer& operator=(defaulted_buffer&&) = default;

        size_t size() const { return data_.size(); }
        const int* data() const { return data_.data(); }

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
        const logged_buffer b{std::move(a)};
        cout << " 1| a.data=" << a.data() << ", b.data=" << b.data() << '\n';

        defaulted_buffer c{1000};
        const defaulted_buffer d{std::move(c)};
        cout << " 2| c.data=" << c.data() << ", d.data=" << d.data() << '\n';

        /* -- .Q&A -- !![Which special members switch off the generated moves?](#a-608) */
    }

    /* --- .`record` and `sure_record` ---
     * Two classes that report copies and moves - the same, except for one word: the move constructor of `sure_record`
     * is `noexcept`. `std::move(other.name_)` moves the `string` member: its characters change the owner, too.
     */

    class record {
    public:
        explicit record(const string& name) : name_{name} {}
        record(const record& other) : name_{other.name_} { cout << " a| - copied " << name_ << '\n'; }
        record(record&& other) : name_{std::move(other.name_)} { cout << " b| - moved " << name_ << '\n'; }

    private:
        string name_;
    };

    class sure_record {
    public:
        explicit sure_record(const string& name) : name_{name} {}
        sure_record(const sure_record& other) : name_{other.name_} { cout << " c| - copied " << name_ << '\n'; }
        // note the `noexcept`
        sure_record(sure_record&& other) noexcept : name_{std::move(other.name_)} { cout << " d| - moved " << name_ << '\n'; }

    private:
        string name_;
    };

    /* --- `grow_a_vector` ---
     * When a `vector` is full, it allocates a bigger block and brings its elements over. With `record`, it copies them
     * - although a move constructor is there. With `sure_record`, it moves them.
     * The reason is a promise of `push_back`: if something throws, the `vector` stays as it was. A copy leaves the old
     * block intact; if a copy fails, the new block is thrown away. A move empties the old elements - if the third move
     * threw, two elements would be gone, and there is no way back. So the `vector` moves only what cannot throw:
     * `noexcept`, checked at compile time.
     * - !![#noexcept]
     */
    void grow_a_vector() {
        print_function_header();

        vector<record> records;
        for (const string name : {"a", "b", "c"}) {
            cout << " 1| emplace_back(" << name << ")\n";
            records.emplace_back(name);
        }

        vector<sure_record> sure_records;
        for (const string name : {"a", "b", "c"}) {
            cout << " 2| emplace_back(" << name << ")\n";
            sure_records.emplace_back(name);
        }
    }

}

/* --- `main` --- */
int main() {
    pass_an_rvalue_on();
    try_to_move_a_const();
    lose_the_moves();
    grow_a_vector();

    return EXIT_SUCCESS;
}
