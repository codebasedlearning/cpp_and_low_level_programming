// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - `push_back(tracer{"a"})` builds a temporary and moves it into the `vector`; `emplace_back("a")` builds the
 *   element in place, in the `vector`'s own memory - one constructor, no move.
 * - `emplace_back` is not magic: given an existing object, it copies, as `push_back` does.
 * - How it works: placement `new` (see previous snippets) at the end of the capacity, with the arguments passed on
 *   unchanged - forwarding.
 * - `T&&` in a template is a forwarding reference: it binds to lvalues and rvalues, and `std::forward<T>` passes each
 *   on as what it was. Without it, a named parameter is always an lvalue.
 */

#include <iostream>
#include <string>
#include <vector>
#include <memory>                           // for construct_at, destroy_at
#include <utility>                          // for forward, move
#include <cstdlib>
#include <cbl/printing.hpp>

using std::cout, std::string, std::vector;


/* ---- Content ---- */

namespace {

    /* --- `tracer` --- Reports how it was made, and when it dies. */
    class tracer {
    public:
        explicit tracer(const string& name) : name_{name} { cout << " a|   constructed " << name_ << '\n'; }
        tracer(const tracer& other) : name_{other.name_} { cout << " b|   copied " << name_ << '\n'; }
        tracer(tracer&& other) noexcept : name_{std::move(other.name_)} { cout << " c|   moved " << name_ << '\n'; }
        ~tracer() { cout << " d|   destroyed " << (name_.empty() ? "(moved-from)" : name_) << '\n'; }

        tracer& operator=(const tracer&) = delete;
        tracer& operator=(tracer&&) = delete;

    private:
        string name_;
    };

    /* --- `compare_push_and_emplace` ---
     * `reserve` first, so that no reallocation moves the elements in between. Then:
     * - `push_back(tracer{"a"})`: constructed, moved into the `vector`, the temporary destroyed at the `;`.
     * - `emplace_back("b")`: only constructed - `"b"` is passed to the constructor, which runs in the `vector`.
     * - `emplace_back(c)` with an existing `tracer`: copied. The arguments decide which constructor runs.
     */
    void compare_push_and_emplace() {
        print_function_header();

        vector<tracer> v{};
        v.reserve(4);
        cout << " 1| push_back(tracer{\"a\"})\n";
        v.push_back(tracer{"a"});
        cout << " 2| emplace_back(\"b\")\n";
        v.emplace_back("b");
        const tracer c{"c"};
        cout << " 3| emplace_back(c)\n";
        v.emplace_back(c);
        cout << " 4| end of function\n";
    }

    /* --- `kind` --- Two overloads, as in the follow-up on moving: which one is called tells the category. */
    void kind(const string& s) { cout << " e|   lvalue: " << s << '\n'; }
    void kind(string&& s) { cout << " f|   rvalue: " << s << '\n'; }

    /* --- `relay` and `relay_forwarded` ---
     * `T&&` with a deduced `T` is not an rvalue reference but a forwarding reference: for an lvalue argument, `T` is
     * `string&`, and `string& &&` collapses to `string&`; for an rvalue, `T` is `string`. Inside, `s` has a name, so it
     * is an lvalue either way - `relay` always calls the lvalue overload. `std::forward<T>(s)` casts back to what the
     * argument was: an rvalue only if `T` is not a reference.
     * - !![#value-categories]
     */
    template <typename T>
    void relay(T&& s) {
        kind(s);
    }

    template <typename T>
    void relay_forwarded(T&& s) {
        kind(std::forward<T>(s));
    }

    /* --- `forward_arguments` --- The same two calls through both relays. */
    void forward_arguments() {
        print_function_header();

        string name{"Naima"};
        cout << " 1| relay(name), relay(string{...})\n";
        relay(name);
        relay(string{"Giant Steps"});
        cout << " 2| relay_forwarded(name), relay_forwarded(string{...})\n";
        relay_forwarded(name);
        relay_forwarded(string{"Giant Steps"});
    }

    /* --- `slot` and `emplace_into` ---
     * The core of `emplace_back`, for one element and one argument: raw bytes of the right size and alignment - the
     * capacity - and `construct_at` with the forwarded argument. The real `emplace_back` takes any number of arguments
     * (`Args&&... args`, a variadic template) and forwards them all the same way.
     */
    struct slot {
        alignas(tracer) unsigned char bytes[sizeof(tracer)];
    };

    template <typename Arg>
    tracer* emplace_into(slot& s, Arg&& arg) {
        return std::construct_at(reinterpret_cast<tracer*>(s.bytes), std::forward<Arg>(arg));
    }

    /* --- `emplace_by_hand` ---
     * With a `string` argument, the `tracer` is constructed in the slot - nothing else. With a `tracer` rvalue, it is
     * moved in, with an lvalue copied: the forwarded argument picks the constructor. `destroy_at` ends each life.
     */
    void emplace_by_hand() {
        print_function_header();

        slot first{};
        slot second{};
        cout << " 1| emplace_into(first, string{\"x\"})\n";
        tracer* x{emplace_into(first, string{"x"})};
        const tracer y{"y"};
        cout << " 2| emplace_into(second, y)\n";
        tracer* y_copy{emplace_into(second, y)};
        std::destroy_at(y_copy);
        std::destroy_at(x);
        cout << " 3| end of function\n";
    }

}

/* --- When to use which ---
 * `emplace_back(args)` when the arguments are what the constructor takes - it saves the temporary and its move.
 * `push_back(obj)` when you have the object already - the same work, and the intent is clearer. For `int`s and other
 * small types the difference is nothing; for `explicit` constructors, `emplace_back` calls them without the word
 * `explicit` getting in the way - one reason some guidelines prefer `push_back`.
 */

/* --- `main` --- */
int main() {
    compare_push_and_emplace();
    forward_arguments();
    emplace_by_hand();

    return EXIT_SUCCESS;
}
