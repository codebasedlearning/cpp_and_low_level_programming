// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - The member initializer list initializes the members; the body can only assign afterwards.
 * - The bytes of a local object exist before its constructor runs - the constructor gives them a meaning.
 * - Members are initialized in declaration order - not in the order of the list.
 * - Default member initializers, and `= default` for the default constructor that disappeared.
 * - A one-argument constructor is also a conversion - it silently builds a temporary object. `explicit` stops it.
 */

#include <iostream>
#include <string>
#include <cstdlib>
#include <cbl/printing.hpp>

using std::cout, std::string;


/* ---- Content ---- */

namespace {

    /* --- `label` --- A member that only reports what happens to it. */
    class label {
    public:
        label() {
            cout << " a|   label: default-constructed, empty\n";
        }

        label(const string& text) {
            cout << " b|   label: constructed with '" << text << "'\n";
        }

        void set(const string& text) {
            cout << " c|   label: set to '" << text << "'\n";
        }
    };

    /* --- `assigned_badge` --- Sets its member in the body - as in the preparation. */
    class assigned_badge {
    public:
        assigned_badge(const string& name) {
            cout << " d|   assigned_badge: body starts\n";
            name_.set(name);
        }

    private:
        label name_;
    };

    /* --- `initialized_badge` ---
     * The part between `:` and `{` is the member initializer list: for each member, the value it is built from.
     */
    class initialized_badge {
    public:
        initialized_badge(const string& name) : name_{name} {
            cout << " e|   initialized_badge: body starts\n";
        }

    private:
        label name_;
    };

    /* --- `initialize_or_assign` ---
     * Before the body of a constructor runs, every member is constructed already. If you do not say how, it is
     * default-constructed - and the body can only overwrite it: two steps. The member initializer list builds it with
     * the right value right away: one step. For an `int` the difference is small, for a `string` it may be an
     * allocation - and for a `const` member or a reference there is no second step at all: they can only be
     * initialized.
     * From now on, members are initialized in the list.
     * - !![#member-initializer-list]
     */
    void initialize_or_assign() {
        print_function_header();

        cout << " 1| assigned in the body\n";
        const assigned_badge a{"Ada"};
        cout << " 2| initialized in the list\n";
        const initialized_badge b{"Grace"};
    }

    /* --- `tower` --- It prints where it is born and where it dies. */
    class tower {
    public:
        tower(const int floors) : floors_{floors} {
            cout << " f|   tower(" << floors_ << ") constructed at " << this << '\n';
        }

        ~tower() {
            cout << " g|   tower(" << floors_ << ") destroyed at " << this << '\n';
        }

        int floors() const { return floors_; }

    private:
        int floors_;
    };

    /* --- `watch_the_birth` ---
     * The memory of `eiffel` is part of the stack frame - reserved when the function starts, like the memory of an
     * `int`. The constructor does not get memory, it gets an address, `this`, and fills the bytes there.
     * - !![#constructor]
     */
    void watch_the_birth() {
        print_function_header();

        cout << " 1| before the constructor\n";
        const tower eiffel{3};
        cout << " 2| &eiffel=" << &eiffel << ", floors=" << eiffel.floors() << '\n';

        /* -- .Before and after. --
         * Set a breakpoint on ` 1|`, and look at `&eiffel` in the memory view - the address is valid already. The 4
         * bytes contain whatever was there. Step over the next two lines, and they contain 3.
         */
    }

    /* --- `interval` --- `end_` is computed from `begin_` - that works only because `begin_` is declared first. */
    class interval {
    public:
        interval(const int begin, const int length) : begin_{begin}, end_{begin_ + length} {}

        int begin() const { return begin_; }
        int end() const { return end_; }

    private:
        int begin_;                         // initialized first, because it is declared first
        int end_;
    };

    /* --- `initialize_in_order` ---
     * The members are initialized in the order in which they are declared in the class. The order in the list does not
     * matter - write it in the same order, so that it reads as it runs.
     */
    void initialize_in_order() {
        print_function_header();

        const interval i{10, 5};
        cout << " 1| [" << i.begin() << ", " << i.end() << ")\n";

        /* -- .Swap the declarations. --
         * Swap the lines `int begin_;` and `int end_;` and build again. The compiler warns (`-Wreorder`), and now
         * `end_` is initialized first - from `begin_`, which has no value yet. Undefined behavior: run it as Debug and
         * as Release.
         */

        /* -- .Q&A -- !![Why does the order of the list not count?](#a-302) */
    }

    /* --- `sensor` ---
     * Default member initializers: a value right at the declaration, used by every constructor that does not set the
     * member itself.
     */
    class sensor {
    public:
        sensor() = default;                 // the default constructor, as the compiler writes it
        explicit sensor(const int id) : id_{id} {}      // `explicit`: see below

        void show() const {
            cout << " h|   id=" << id_ << ", value=" << value_ << '\n';
        }

    private:
        int id_{-1};
        double value_{0.0};
    };

    /* --- `use_default_member_initializers` ---
     * Without any constructor, the compiler writes a default constructor. As soon as you declare one yourself, it does
     * not - delete the line `sensor() = default;` and `sensor s;` no longer compiles.
     * - !![#default-delete]
     */
    void use_default_member_initializers() {
        print_function_header();

        const sensor s;
        s.show();
        const sensor t{7};
        t.show();
    }

    /* --- `visit` --- Takes a `tower` - but see what it accepts. */
    void visit(const tower& t) {
        cout << " i|   visiting a tower with " << t.floors() << " floors\n";
    }

    /* --- `convert_silently` ---
     * `tower(const int floors)` also says: an `int` can become a `tower`. So `eiffel = 4` compiles - the compiler
     * builds a temporary `tower` from the 4, assigns it, and destroys it at the `;`. Compare the addresses: there is a
     * second object you never wrote.
     * - !![#temporary]
     */
    void convert_silently() {
        print_function_header();

        tower eiffel{3};
        cout << " 1| &eiffel=" << &eiffel << ", before eiffel = 4\n";
        eiffel = 4;
        cout << " 2| floors=" << eiffel.floors() << '\n';

        visit(5);                           // a temporary `tower` again - does a tower with 5 floors exist here?
        cout << " 3| end of function\n";
    }

    /* --- `sky_tower` --- The same as `tower`, with `explicit`. */
    class sky_tower {
    public:
        explicit sky_tower(const int floors) : floors_{floors} {
            cout << " j|   sky_tower(" << floors_ << ") constructed at " << this << '\n';
        }

        ~sky_tower() {
            cout << " k|   sky_tower(" << floors_ << ") destroyed at " << this << '\n';
        }

        int floors() const { return floors_; }

    private:
        int floors_;
    };

    /* --- `forbid_implicit_conversion` ---
     * `explicit` forbids using the constructor as a silent conversion. The temporary is still possible - but now it is
     * written in the code, where a reader sees it.
     * Rule of thumb: a constructor with one argument is `explicit`, unless the conversion is the point.
     * - !![#explicit]
     */
    void forbid_implicit_conversion() {
        print_function_header();

        sky_tower burj{3};
        // burj = 4;                        // compiler error: no conversion from `int`
        burj = sky_tower{4};
        cout << " 1| floors=" << burj.floors() << '\n';
    }

}

/* --- `main` --- */
int main() {
    initialize_or_assign();
    watch_the_birth();
    initialize_in_order();
    use_default_member_initializers();
    convert_silently();
    forbid_implicit_conversion();

    return EXIT_SUCCESS;
}
