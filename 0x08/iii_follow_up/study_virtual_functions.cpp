// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - `virtual` in the base is enough - an override is virtual, too. Write `override` in the derived class: the compiler
 *   checks that the function really overrides one, and a missing `const` becomes an error instead of a new function.
 * - `final` on a function: no class further down may override it.
 * - Default arguments are taken from the static type, the function from the dynamic type. Never give an override other
 *   defaults.
 * - An override may return a pointer or a reference to a derived class - a covariant return type.
 * - A pure virtual function may have a body, called by its full name. A pure virtual destructor must have one.
 * - An interface in C++ is a convention, not a keyword: only pure virtual functions, a virtual destructor, no data.
 */

#include <iostream>
#include <string>
#include <memory>
#include <cstdlib>
#include <cbl/printing.hpp>

using std::cout, std::string, std::unique_ptr;


/* ---- Content ---- */

namespace {

    /* --- `sensor` and `thermometer` ---
     * `read` is `const` in `sensor`. Had `thermometer` forgotten the `const` - `double read() override` - the compiler
     * would refuse: `read` marked `override`, but does not override. Without `override`, the same line compiles: a new,
     * non-virtual function that hides `sensor::read` - and a `const sensor&` calls the old one. gcc 13 and clang warn
     * about the hidden function (`-Woverloaded-virtual`, in `-Wall`); `override` makes it an error everywhere.
     * `virtual` is not repeated in the derived class: `override` says more. `final` says: this is the last override -
     * nothing derived from `thermometer` may override `read` again.
     * - !![#override]
     */
    class sensor {
    public:
        virtual ~sensor() = default;
        virtual double read() const { return 0.0; }
    };

    class thermometer : public sensor {
    public:
        double read() const final { return 21.5; }
        // double read() override { ... }   // compiler error: does not override - `const` is missing
    };

    /* --- `check_the_override` --- The override, through the base. */
    void check_the_override() {
        print_function_header();

        const thermometer t;
        const sensor& s{t};
        cout << " 1| s.read()=" << s.read() << '\n';
    }

    /* --- `greeter` and `loud_greeter` --- Two defaults for the same parameter. */
    class greeter {
    public:
        virtual ~greeter() = default;
        virtual string greet(const string& who = "world") const { return "hello, " + who; }
    };

    class loud_greeter : public greeter {
    public:
        string greet(const string& who = "EVERYONE") const override { return "HELLO, " + who + "!"; }
    };

    /* --- `bind_default_arguments_statically` ---
     * `g.greet()` calls `loud_greeter::greet` - the dynamic type - with "world" - the default of `greeter`, the static
     * type. The compiler fills in the default argument at the call, and it looks at the type it sees; the vtable holds
     * functions, not defaults. The result mixes two classes. If an override needs a default, it takes the one of the
     * base - or none.
     */
    void bind_default_arguments_statically() {
        print_function_header();

        const loud_greeter l;
        const greeter& g{l};
        cout << " 1| l.greet()=" << l.greet() << '\n';
        cout << " 2| g.greet()=" << g.greet() << '\n';
    }

    /* --- `document` and `report` ---
     * `document::copy` returns a `document*`, `report::copy` a `report*` - a different return type, and still an
     * override: a pointer to a derived class may replace a pointer to the base. Through a `report`, you get a `report*`
     * without a cast. The returned object is new - its owner is the caller, who puts it into a `unique_ptr` at once.
     * With `unique_ptr` as the return type, this does not work: `unique_ptr<report>` does not derive from
     * `unique_ptr<document>` - see the next follow-up.
     */
    class document {
    public:
        virtual ~document() = default;
        virtual document* copy() const { return new document{*this}; }
        virtual string kind() const { return "document"; }
    };

    class report : public document {
    public:
        report* copy() const override { return new report{*this}; }
        string kind() const override { return "report"; }
        int pages() const { return 12; }
    };

    /* --- `return_a_covariant_type` --- A `report*` from `copy`, without a cast. */
    void return_a_covariant_type() {
        print_function_header();

        const report r;
        const unique_ptr<report> copy{r.copy()};
        const document& d{r};
        const unique_ptr<document> another{d.copy()};
        cout << " 1| copy->pages()=" << copy->pages() << ", another->kind()=" << another->kind() << '\n';
    }

    /* --- `alert` and `fire_alert` ---
     * `alert::message` is pure virtual - `alert` is abstract, every derived class must override `message` - and it has
     * a body anyway, defined outside the class. An override may call it by its full name: a default part to build on,
     * which nobody can forget to override.
     */
    class alert {
    public:
        virtual ~alert() = default;
        virtual string message() const = 0;
    };

    string alert::message() const {
        return "ALERT";
    }

    class fire_alert : public alert {
    public:
        string message() const override { return alert::message() + ": fire"; }
    };

    /* --- `define_a_pure_virtual_function` --- The body of a pure virtual function, called by name. */
    void define_a_pure_virtual_function() {
        print_function_header();

        const fire_alert f;
        const alert& a{f};
        cout << " 1| a.message()=" << a.message() << '\n';
        // const alert plain;               // compiler error: `alert` is abstract, body or not
    }

    /* --- `marker` and `tagged` ---
     * Sometimes a class should be abstract, but has no function that could be pure virtual. Then the destructor is made
     * pure virtual, `= 0`. It must have a body all the same: every destructor of a derived class calls it (see previous
     * snippets). Without the line `marker::~marker() = default;` below, the linker reports an undefined reference to
     * `marker::~marker()`.
     */
    class marker {
    public:
        virtual ~marker() = 0;
    };

    marker::~marker() = default;

    class tagged : public marker {};

    /* --- `need_a_pure_virtual_destructor` --- An abstract class without an abstract function. */
    void need_a_pure_virtual_destructor() {
        print_function_header();

        const tagged t;
        // const marker m;                  // compiler error: `marker` is abstract
        cout << " 1| sizeof(tagged)=" << sizeof(tagged) << '\n';
    }

    /* --- `drawable` and `clickable` ---
     * Two interfaces, as Java or C# would declare them with `interface`: only pure virtual functions, a public virtual
     * destructor, no data members, no constructor. C++ has no keyword for it - an abstract class that follows these
     * rules is an interface. A class may implement several: that is multiple inheritance, in its harmless form (see the
     * session). Their price: a vptr per interface in every object.
     * - !![#abstract-class]
     */
    class drawable {
    public:
        virtual ~drawable() = default;
        virtual string draw() const = 0;
    };

    class clickable {
    public:
        virtual ~clickable() = default;
        virtual string click() = 0;
    };

    class toggle : public drawable, public clickable {
    public:
        string draw() const override { return on_ ? "[x]" : "[ ]"; }
        string click() override {
            on_ = !on_;
            return on_ ? "on" : "off";
        }

    private:
        bool on_{false};
    };

    /* --- `use_interfaces` --- One object, used through two interfaces. */
    void use_interfaces() {
        print_function_header();

        toggle t;
        clickable& c{t};
        const drawable& d{t};
        cout << " 1| " << d.draw() << ", click: " << c.click() << ", " << d.draw() << '\n';
        cout << " 2| sizeof(toggle)=" << sizeof(toggle) << '\n';

        /* -- .Q&A -- !![Why must an interface have a virtual destructor?](#a-814) */
    }

}

/* --- `main` --- */
int main() {
    check_the_override();
    bind_default_arguments_statically();
    return_a_covariant_type();
    define_a_pure_virtual_function();
    need_a_pure_virtual_destructor();
    use_interfaces();

    return EXIT_SUCCESS;
}
