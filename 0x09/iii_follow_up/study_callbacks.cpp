// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - The syntax of function pointers - as a variable, a parameter, a return type - and why `using` helps.
 * - `std::function` holds a function, a lambda or a function object - one at a time, and it can be empty.
 * - The standard library has function objects for the operators: `std::less<>`, `std::greater<>`, `std::plus<>`.
 * - `std::invoke` calls anything callable - even a pointer to a member.
 * - A callback is a function that you hand over, for somebody else to call later. From the old UI library: a
 *   button with a list of callbacks - and why a widget that registers `[this]` must not be copied or moved.
 */

#include <iostream>
#include <string>
#include <vector>
#include <functional>                       // for function, invoke, less, greater, plus
#include <algorithm>
#include <numeric>
#include <utility>
#include <cstdlib>
#include <cbl/printing.hpp>

using std::cout, std::string, std::vector, std::function;


/* ---- Content ---- */

namespace {

    /* --- `add_one`, `negate` and `unary` --- Two functions, and the type of a pointer to them. */
    int add_one(const int x) {
        return x + 1;
    }

    int negate(const int x) {
        return -x;
    }

    using unary = int (*)(int);

    /* --- `apply_twice` and `pick` ---
     * A function pointer as a parameter, and as a return type. Without `using`, `pick` would be declared as
     *     int (*pick(char op))(int)
     * "`pick` is a function taking a `char`, returning a pointer to a function taking an `int` and returning an
     * `int`" - read from the name outwards. That is what C programmers write; `using` is kinder, and `auto` hides it.
     * - !![#std-function]
     */
    int apply_twice(const unary f, const int x) {
        return f(f(x));
    }

    unary pick(const char op) {
        return op == '-' ? negate : add_one;
    }

    /* --- `write_function_pointer_types` ---
     * `int (*const raw)(int)` - a `const` pointer to a function, written out; the `const` after the `*` belongs to the
     * pointer (see previous snippets: read from right to left). The same with the alias, and with `auto`.
     */
    void write_function_pointer_types() {
        print_function_header();

        int (*const raw)(int){add_one};
        const unary f{negate};
        const auto g{pick('+')};
        cout << " 1| raw(1)=" << raw(1) << ", f(1)=" << f(1) << ", g(1)=" << g(1) << '\n';
        cout << " 2| apply_twice(add_one, 5)=" << apply_twice(add_one, 5) << ", pick('-')(5)=" << pick('-')(5) << '\n';
    }

    /* --- `doubler` --- A function object: a class with an `operator()` (see the session). */
    struct doubler {
        int operator()(const int x) const { return 2 * x; }
    };

    /* --- `use_std_function` ---
     * One `std::function<int(int)>`, four things in it, one after the other: a function, a lambda, a lambda with a
     * capture, a function object. A default-constructed `std::function` is empty - `if (f)` asks, and so does
     * `f == nullptr`. Assigning `nullptr` empties it again.
     * A `std::function` as a data member is the usual way to store a callback of unknown kind - at the price of the
     * session: its size, maybe an allocation, and a call that is not inlined.
     */
    void use_std_function() {
        print_function_header();

        function<int(int)> f;
        cout << " 1| empty: " << (f == nullptr);
        f = add_one;
        cout << ", function: " << f(10);
        f = [](const int x) { return x * x; };
        cout << ", lambda: " << f(10);
        int offset{100};                    // not `const` - a constant need not be captured (see the session)
        f = [offset](const int x) { return x + offset; };
        cout << ", with a capture: " << f(10);
        f = doubler{};
        cout << ", function object: " << f(10) << '\n';
        f = nullptr;
        cout << " 2| empty again: " << !f << '\n';
    }

    /* --- `use_standard_function_objects` ---
     * `<functional>` has a function object for every operator: `std::less<>` calls `<`, `std::greater<>` `>`,
     * `std::plus<>` `+`, `std::multiplies<>` `*`. The empty `<>` lets them compare or add whatever they get - they
     * are templates with a template `operator()`, like a generic lambda. `sort` uses `std::less<>` when you pass
     * nothing; with `std::greater<>`, it sorts backwards - no lambda needed.
     */
    void use_standard_function_objects() {
        print_function_header();

        vector<int> numbers{5, 2, 8, 1, 9};
        std::sort(numbers.begin(), numbers.end(), std::greater<>{});
        cout << " 1| sorted backwards:";
        for (const int n : numbers) {
            cout << ' ' << n;
        }
        cout << '\n';
        cout << " 2| product: " << std::accumulate(numbers.begin(), numbers.end(), 1, std::multiplies<>{})
             << ", less<>{}(1, 2)=" << std::less<>{}(1, 2) << '\n';
    }

    /* --- `person` --- A name, and a member function. */
    struct person {
        string name;

        string greet(const string& other) const { return name + " greets " + other; }
    };

    /* --- `invoke_anything` ---
     * `std::invoke(f, args...)` calls `f` with the arguments - whatever `f` is. For a function or a lambda, that is
     * `f(args...)`. For a pointer to a member function, `&person::greet`, the first argument is the object:
     * `std::invoke(&person::greet, p, "Sam")` is `p.greet("Sam")`. For a pointer to a data member, `&person::name`, it
     * reads the member. The algorithms of `std::ranges` call their projections this way - that is how `&person::name`
     * could be a key (see the other study).
     * A pointer to a member function is a creature of its own: see the `tinker_` file.
     */
    void invoke_anything() {
        print_function_header();

        const person p{"Kim"};
        cout << " 1| " << std::invoke(add_one, 1) << ", " << std::invoke([](const int x) { return -x; }, 1) << '\n';
        cout << " 2| " << std::invoke(&person::greet, p, "Sam") << ", name: " << std::invoke(&person::name, p) << '\n';
    }

    /* --- `event` ---
     * From the old UI library: a list of callbacks for one kind of event. `add` appends one, `operator()` calls all of
     * them, in order, with the object that raised the event. The callbacks are `std::function`s - a button does not
     * know, and does not care, what the callers hand it.
     */
    template <typename T>
    class event {
    public:
        void add(function<void(const T&)> handler) {
            handlers_.push_back(std::move(handler));
        }

        void operator()(const T& sender) const {
            for (const function<void(const T&)>& handler : handlers_) {
                handler(sender);
            }
        }

    private:
        vector<function<void(const T&)>> handlers_;
    };

    /* --- `widget` ---
     * The base of all widgets. Copy and move are deleted: other parts of a program hold the address of a widget - in
     * callbacks with `[this]`, in the list of children of a window. If a widget could be copied or moved, those
     * addresses would point to the old place (see the session). So it cannot. A widget lives where it was created, and
     * is passed by reference.
     */
    class widget {
    public:
        widget() = default;
        virtual ~widget() = default;

        widget(const widget&) = delete;
        widget& operator=(const widget&) = delete;
        widget(widget&&) = delete;
        widget& operator=(widget&&) = delete;
    };

    /* --- `button` --- A widget with a text and an event. `click` raises it - usually, the mouse would. */
    class button : public widget {
    public:
        explicit button(string text) : text_{std::move(text)} {}

        const string& text() const { return text_; }
        void click() const { on_click(*this); }

        event<button> on_click;

    private:
        string text_;
    };

    /* --- `command_button` ---
     * A button with a command of its own. The constructor registers a callback with `[this]` - the address of this
     * very object, which then calls `command_`. Safe, because a `command_button` cannot be copied or moved: the
     * address stays valid as long as the button lives - and the callback lives inside the button.
     */
    class command_button : public button {
    public:
        command_button(string text, function<void(const button&)> command)
            : button{std::move(text)}, command_{std::move(command)} {
            on_click.add([this](const button& sender) { command_(sender); });
        }

    private:
        function<void(const button&)> command_;
    };

    /* --- `click_a_button` ---
     * Two handlers for a plain button, and a command button. Every click calls whatever was registered.
     * `const button copy{ok};` does not compile - and that is the point.
     */
    void click_a_button() {
        print_function_header();

        button ok{"OK"};
        int clicks{0};
        ok.on_click.add([](const button& b) { cout << " 1|   clicked: " << b.text() << '\n'; });
        ok.on_click.add([&clicks](const button&) { ++clicks; });
        ok.click();
        ok.click();
        cout << " 2| clicks=" << clicks << '\n';

        const command_button save{"Save", [](const button& b) { cout << " 3|   command from " << b.text() << '\n'; }};
        save.click();
        // const button copy{ok};           // compiler error: the copy constructor of `widget` is deleted
    }

}

/* --- `main` --- */
int main() {
    write_function_pointer_types();
    use_std_function();
    use_standard_function_objects();
    invoke_anything();
    click_a_button();

    return EXIT_SUCCESS;
}
