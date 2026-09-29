// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - The copy constructor builds a new object from an existing one; the copy assignment overwrites an existing one.
 * - `tracer c = a;` looks like an assignment, but it is a construction.
 * - The generated copy copies member by member - a `string` member copies its characters into a new heap block.
 * - Every pass by value calls the copy constructor - the copies measured in unit 0x02, now visible.
 * - A local object returned by value is built directly in the caller's place - no copy.
 */

#include <iostream>
#include <string>
#include <cstdlib>
#include <cbl/printing.hpp>

using std::cout, std::string;


/* ---- Content ---- */

namespace {

    /* --- `tracer` ---
     * A class that reports every construction, copy and destruction. A copy gets a `'` added to its name, so you can
     * tell it from the original.
     * - !![#copy-constructor]
     */
    class tracer {
    public:
        explicit tracer(const string& name) : name_{name} {
            cout << " a|   " << name_ << ": constructed at " << this << '\n';
        }

        // The copy constructor: a new object, built from `other`.
        tracer(const tracer& other) : name_{other.name_ + "'"} {
            cout << " b|   " << name_ << ": copy-constructed at " << this << " from " << &other << '\n';
        }

        // The copy assignment: `*this` exists already and gets the state of `other`. It returns `*this`, so that
        // `a = b = c` works as for an `int`.
        tracer& operator=(const tracer& other) {
            cout << " c|   " << name_ << ": assigned from " << other.name_ << '\n';
            name_ = other.name_ + "'";
            return *this;
        }

        ~tracer() {
            cout << " d|   " << name_ << ": destroyed at " << this << '\n';
        }

    private:
        string name_;
    };

    /* --- `construct_or_assign` ---
     * The rule: if a new object is created, it is a constructor - no matter whether the line contains a `=`. An
     * assignment needs an object that exists already.
     */
    void construct_or_assign() {
        print_function_header();

        const tracer a{"a"};
        cout << " 1| tracer b{a};\n";
        const tracer b{a};
        cout << " 2| tracer c = a;\n";
        const tracer c = a;                 // a copy construction, not an assignment
        tracer d{"d"};
        cout << " 3| d = a;\n";
        d = a;                              // an assignment: `d` exists already
        cout << " 4| end of function\n";
    }

    /* --- `contact` ---
     * No copy constructor, no copy assignment - the compiler generates both: member by member.
     */
    class contact {
    public:
        contact(const string& name, const int age) : name_{name}, age_{age} {}

        const void* characters() const { return name_.c_str(); }
        int age() const { return age_; }

    private:
        string name_;
        int age_;
    };

    /* --- `copy_member_by_member` ---
     * The generated copy constructor copies each member with its own copy constructor: for the `int` that is 4 bytes,
     * for the `string` a new heap block with the same characters. Whether a copy is deep or shallow is decided by the
     * members - `string` and `vector` copy deeply.
     * - !![#deep-copy]
     */
    void copy_member_by_member() {
        print_function_header();

        const contact ada{"Ada Lovelace, Countess of Lovelace", 36};
        const contact copy{ada};
        cout << " 1| ada:  at " << &ada << ", characters at " << ada.characters() << '\n';
        cout << " 2| copy: at " << &copy << ", characters at " << copy.characters() << '\n';
    }

    /* --- `visit_by_value` and `visit_by_reference` --- One takes a copy, one does not. */
    void visit_by_value(const tracer t) {
        cout << " e|   visit_by_value: &t=" << &t << '\n';
    }

    void visit_by_reference(const tracer& t) {
        cout << " f|   visit_by_reference: &t=" << &t << '\n';
    }

    /* --- `pass_by_value` ---
     * Passing by value initializes the parameter from the argument - with the copy constructor. And at the end of the
     * function, the copy is destroyed. That is the price of a copy measured in unit 0x02: a constructor and a
     * destructor call, for a `string` member an allocation and a release.
     * - !![#parameter-passing]
     */
    void pass_by_value() {
        print_function_header();

        const tracer v{"v"};
        cout << " 1| by value\n";
        visit_by_value(v);
        cout << " 2| by reference\n";
        visit_by_reference(v);
        cout << " 3| end of function\n";
    }

    /* --- `make_tracer` --- Returns a new object by value. */
    tracer make_tracer(const string& name) {
        return tracer{name};
    }

    /* --- `return_by_value` ---
     * Returning by value looks like a copy: build a `tracer` in `make_tracer`, copy it into `m`. But there is only one
     * construction, and its address is the address of `m`.
     * - !![#copy-elision]
     */
    void return_by_value() {
        print_function_header();

        const tracer m{make_tracer("m")};
        cout << " 1| &m=" << &m << '\n';

        /* -- .Q&A -- !![How can `make_tracer` build an object in the frame of its caller?](#a-303) */

        /* -- .Without elision. --
         * Build with `-fno-elide-constructors` and return a named local instead, `tracer t{name}; return t;` - now you
         * see the copy that the compiler usually saves you.
         */
    }

}

/* --- `main` --- */
int main() {
    construct_or_assign();
    copy_member_by_member();
    pass_by_value();
    return_by_value();

    return EXIT_SUCCESS;
}
