// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - Getters and setters: private data, public access - worth it when they protect a rule.
 * - A getter can return a copy or a `const&` - the question of unit 0x02, once more.
 * - A getter that returns a non-`const` reference makes the member public again.
 * - Instead of a setter per member: member functions with a purpose.
 */

#include <iostream>
#include <string>
#include <stdexcept>
#include <cstdlib>
#include <cbl/printing.hpp>

using std::cout, std::string, std::invalid_argument;


/* ---- Content ---- */

namespace {

    /* --- `account` ---
     * The balance must never be negative. A setter that checks keeps that promise; a public member could not.
     * - !![#getter-setter]
     */
    class account {
    public:
        explicit account(const long balance) : balance_{balance} {}

        // Two common naming styles for a getter: `get_balance()` - or simply `balance()`, the style of the standard
        // library (`size()`, `capacity()`). This course uses the second one.
        long balance() const { return balance_; }

        // A setter with a check. The rule lives in one place, not at every caller.
        void set_balance(const long balance) {
            if (balance < 0) {
                throw invalid_argument{"negative balance"};
            }
            balance_ = balance;
        }

        // Looks convenient - but it hands out the address of the member. Anyone can write through it, without a check.
        long& balance_ref() { return balance_; }

    private:
        long balance_;
    };

    /* --- `use_getters_and_setters` --- */
    void use_getters_and_setters() {
        print_function_header();

        account a{100};
        a.set_balance(50);
        cout << " 1| balance=" << a.balance() << '\n';

        try {
            a.set_balance(-10);
        } catch (const invalid_argument& e) {
            cout << " 2| rejected: " << e.what() << '\n';
        }

        a.balance_ref() = -10;              // no check - the rule is broken
        cout << " 3| balance=" << a.balance() << " - as good as a public member\n";
    }

    /* --- `person` --- A `string` member, and two ways to return it. */
    class person {
    public:
        explicit person(const string& name) : name_{name} {}

        // Returns a copy: safe, but for a long name every call allocates.
        string name_copy() const { return name_; }

        // Returns a reference to the member: no copy - but it is only valid as long as the object lives.
        const string& name() const { return name_; }

    private:
        string name_;
    };

    /* --- `return_copy_or_reference` ---
     * Compare the addresses of the characters: the copy has its own, the reference shows the member's. As in unit 0x02:
     * return `const&` for bigger members that are only read - and do not keep the reference longer than the object.
     * - !![#parameter-passing]
     */
    void return_copy_or_reference() {
        print_function_header();

        const person ada{"Ada Lovelace, Countess of Lovelace"};
        const string copy{ada.name_copy()};
        const string& ref{ada.name()};
        const void* member{ada.name().c_str()};
        const void* copied{copy.c_str()};
        const void* referenced{ref.c_str()};
        cout << " 1| member characters at " << member << '\n';
        cout << " 2| copy:      " << copied << '\n';
        cout << " 3| reference: " << referenced << '\n';

        /* -- .Q&A -- !![When does the reference returned by `name()` dangle?](#a-306) */
    }

    /* --- `contact` ---
     * No setter for each member. The member functions say what may happen to a contact - and keep name and phone
     * number consistent, e.g. nobody can set an empty name.
     */
    class contact {
    public:
        contact(const string& name, const string& phone) : name_{name}, phone_{phone} {}

        const string& name() const { return name_; }
        const string& phone() const { return phone_; }

        void rename(const string& name) {
            if (name.empty()) {
                throw invalid_argument{"empty name"};
            }
            name_ = name;
        }

        void move_to_new_phone(const string& phone) {
            phone_ = phone;
        }

    private:
        string name_;
        string phone_;
    };

    /* --- `use_functions_with_a_purpose` ---
     * A class with a getter and a setter for every member is a `struct` with extra typing. Ask what the users of the
     * class want to do, and offer exactly that.
     */
    void use_functions_with_a_purpose() {
        print_function_header();

        contact c{"Grace Hopper", "0241 12345"};
        c.move_to_new_phone("0241 54321");
        cout << " 1| " << c.name() << ", " << c.phone() << '\n';
    }

}

/* --- `main` --- */
int main() {
    use_getters_and_setters();
    return_copy_or_reference();
    use_functions_with_a_purpose();

    return EXIT_SUCCESS;
}
