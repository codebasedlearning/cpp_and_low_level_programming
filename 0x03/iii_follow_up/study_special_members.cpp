// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - The special member functions: default constructor, copy constructor, copy assignment, destructor - and two move
 *   operations, which come later.
 * - The compiler writes them for you - member by member - unless you declare them yourself.
 * - `= default` asks for the compiler's version, `= delete` forbids a function.
 * - Rule of Zero, Rule of Three.
 */

#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <cbl/printing.hpp>
#include <cbl/stopwatch.hpp>

using std::cout, std::string, std::vector;


/* ---- Content ---- */

namespace {

    /* --- `song` ---
     * No special member at all - and yet it can be created, copied, assigned and destroyed. The compiler generates:
     * - `song()` - the default constructor, it default-constructs each member,
     * - `song(const song&)` - the copy constructor, it copies each member,
     * - `song& operator=(const song&)` - the copy assignment, it assigns each member,
     * - `~song()` - the destructor, it destroys each member, in reverse order.
     * Since C++11 there are two more, the move constructor and the move assignment: see future snippets.
     * - !![#rule-of-zero]
     */
    class song {
    public:
        const string& title() const { return title_; }
        void rename(const string& title) { title_ = title; }

    private:
        string title_{"untitled"};
        vector<int> ratings_{};
    };

    /* --- `use_generated_members` ---
     * Everything works, and everything is right: `string` and `vector` manage their memory themselves, so copying
     * member by member copies deeply. That is the Rule of Zero: use members that clean up after themselves, and write
     * none of the special members.
     */
    void use_generated_members() {
        print_function_header();

        song a;                             // default constructor
        a.rename("Imagine");
        const song b{a};                    // copy constructor
        song c;
        c = b;                              // copy assignment
        cout << " 1| a=" << a.title() << ", b=" << b.title() << ", c=" << c.title() << '\n';
    }                                       // three destructors

    /* --- `sample` ---
     * Once you declare any constructor, the generated default constructor is gone. `= default` brings it back -
     * explicitly, so a reader sees that it is wanted.
     * - !![#default-delete]
     */
    class sample {
    public:
        sample() = default;
        explicit sample(const double value) : value_{value} {}

        double value() const { return value_; }

    private:
        double value_{0.0};
    };

    /* --- `use_default` --- */
    void use_default() {
        print_function_header();

        const sample s;
        const sample t{2.5};
        cout << " 1| s=" << s.value() << ", t=" << t.value() << '\n';
    }

    /* --- `lap_timer` ---
     * Copying a timer makes no sense - which one is the real one? `= delete` removes the copy operations: using them is
     * a compiler error, not a surprise at runtime.
     */
    class lap_timer {
    public:
        lap_timer() = default;
        lap_timer(const lap_timer&) = delete;
        lap_timer& operator=(const lap_timer&) = delete;

        double lap_ms() {
            const double ms{watch_.elapsed_ms()};
            watch_.reset();
            return ms;
        }

    private:
        stopwatch watch_{};
    };

    /* --- `try_to_copy` --- Does not compile once you remove the `//`. */
    void try_to_copy() {
        print_function_header();

        lap_timer timer;
        cout << " 1| first lap: " << timer.lap_ms() << " ms\n";
        // lap_timer other{timer};          // compiler error: use of deleted function
        // void f(lap_timer t); f(timer);   // the same: passing by value copies
    }

}

/* --- Rule of Three ---
 * If a class needs a destructor of its own, it manages a resource by hand - and then the generated copy operations
 * are almost certainly wrong: they copy the handle, not the resource. So: destructor, copy constructor and copy
 * assignment - all three or none. With `new` and `delete` you will see exactly that go wrong: see future snippets.
 * - Rule of Zero: define none of them - the goal.
 * - Rule of Three: define all of destructor, copy constructor and copy assignment - or `= delete` the copies.
 * - Rule of Five: the same, plus the two move operations.
 */

/* --- `main` --- */
int main() {
    use_generated_members();
    use_default();
    try_to_copy();

    return EXIT_SUCCESS;
}
