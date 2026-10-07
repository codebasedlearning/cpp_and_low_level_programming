// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - Members: `public` for everyone, `protected` for the class and the classes derived from it, `private` for the class
 *   alone.
 * - The access in front of the base - `public`, `protected`, `private` - limits what the users of the derived class
 *   see of the base. Only `public` inheritance means "is a".
 * - `using base::base;` inherits the constructors of the base.
 * - A function in the derived class hides every function of that name in the base - all overloads. `using base::f;`
 *   brings them back.
 * - An override can call the version of the base by its full name - to extend it instead of replacing it.
 * - `final` on a class: nobody may derive from it.
 * - Derive only if a derived object can stand in for a base object everywhere - Liskov's rule. Otherwise: a member.
 */

#include <iostream>
#include <string>
#include <cstdlib>
#include <cbl/printing.hpp>

using std::cout, std::string, std::to_string;


/* ---- Content ---- */

namespace {

    /* --- `widget` ---
     * Three members, three levels of access. `info` is a member function: inside the class, everything is accessible.
     * - !![#access-modifiers]
     */
    class widget {
    public:
        string info() const {
            return "pub=" + to_string(pub) + ", prot=" + to_string(prot) + ", priv=" + to_string(priv);
        }

        int pub{1};

    protected:
        int prot{2};

    private:
        int priv{3};
    };

    /* --- `button` ---
     * `public` inheritance: what is public in `widget` is public in `button`, too. A button sees `pub` and `prot` of
     * its `widget` part - not `priv`: private is private, even for derived classes.
     */
    class button : public widget {
    public:
        string info() const { return "pub=" + to_string(pub) + ", prot=" + to_string(prot); }
    };

    /* --- `text` and `checkbox` ---
     * `protected` inheritance: the public members of `widget` become protected in `text` - `text` sees them, its users
     * do not. `private` inheritance: they become private in `checkbox`, and even classes derived from `checkbox` do not
     * see them. Both hide that there is a `widget` inside: a `text` is not a `widget` for its users - a `text` cannot
     * be passed as a `const widget&`.
     */
    class text : protected widget {
    public:
        string info() const { return "pub=" + to_string(pub) + ", prot=" + to_string(prot); }
    };

    class checkbox : private widget {
    public:
        string info() const { return "pub=" + to_string(pub) + ", prot=" + to_string(prot); }
    };

    /* --- `use_access` ---
     * Two kinds of users: the derived class, and the code that works with an object - here, this function. From
     * outside, only `public` counts, and only through `public` inheritance.
     * Private inheritance means "is implemented in terms of" - but a private member does the same, and says so more
     * clearly. You will rarely see `protected` or `private` inheritance; `public` is the rule.
     */
    void use_access() {
        print_function_header();

        const widget w;
        const button b;
        const text t;
        const checkbox c;
        cout << " 1| w.pub=" << w.pub << ", w.info(): " << w.info() << '\n';
        cout << " 2| b.pub=" << b.pub << ", b.info(): " << b.info() << '\n';
        cout << " 3| t.info(): " << t.info() << ", c.info(): " << c.info() << '\n';
        // cout << w.prot;                  // compiler error: `prot` is protected
        // cout << t.pub;                   // compiler error: `pub` is protected in `text`
        // const widget& as_widget{c};      // compiler error: `widget` is an inaccessible base of `checkbox`

        /* -- .Q&A -- !![`protected` data members - a good idea?](#a-813) */
    }

    /* --- `labeled` and `labeled_button` ---
     * `using labeled::labeled;` - the constructors of the base are the constructors of the derived class, too. Handy
     * when the derived class adds functions, but no data that needs a constructor.
     */
    class labeled {
    public:
        explicit labeled(const string& label) : label_{label} {}
        labeled(const string& label, const int width) : label_{label}, width_{width} {}

        const string& label() const { return label_; }
        int width() const { return width_; }

    private:
        string label_;
        int width_{10};
    };

    class labeled_button : public labeled {
    public:
        using labeled::labeled;

        string render() const { return "[" + label() + "]"; }
    };

    /* --- `inherit_constructors` --- Both constructors of `labeled`, without writing them again. */
    void inherit_constructors() {
        print_function_header();

        const labeled_button ok{"OK"};
        const labeled_button cancel{"Cancel", 20};
        cout << " 1| " << ok.render() << " width " << ok.width() << ", " << cancel.render() << " width "
             << cancel.width() << '\n';
    }

    /* --- `logger` and `file_logger` --- `logger` has two `log` functions, `file_logger` one. */
    class logger {
    public:
        string log(const int code) const { return "code " + to_string(code); }
        string log(const string& message) const { return "message " + message; }
    };

    class file_logger : public logger {
    public:
        string log(const string& message) const { return "file: " + message; }
    };

    class complete_file_logger : public logger {
    public:
        using logger::log;
        string log(const string& message) const { return "file: " + message; }
    };

    /* --- `bring_back_hidden_overloads` ---
     * The compiler looks up a name first, and chooses among the overloads afterwards. It finds `log` in
     * `file_logger` - and stops looking: the two `log`s of `logger` are hidden, both of them, and `log(42)` does not
     * compile - although `logger::log(int)` would fit. `using logger::log;` makes the base's overloads visible in the
     * derived class, next to its own.
     * - !![#overloading]
     */
    void bring_back_hidden_overloads() {
        print_function_header();

        const file_logger f;
        const complete_file_logger c;
        cout << " 1| f.log(\"disk full\")=" << f.log("disk full") << '\n';
        // cout << f.log(42);               // compiler error: `logger::log(int)` is hidden
        cout << " 2| c.log(42)=" << c.log(42) << ", c.log(\"disk full\")=" << c.log("disk full") << '\n';
    }

    /* --- `account` and `savings_account` ---
     * `savings_account::describe` overrides `describe` - and calls the version of `account` by its full name, to add a
     * line instead of repeating it. With the class name in front, the call is not virtual (see the session) - otherwise
     * it would call itself. Java writes `super.describe()`; C++ has no `super`, a class may have several bases.
     */
    class account {
    public:
        explicit account(const int balance) : balance_{balance} {}
        virtual ~account() = default;

        virtual string describe() const { return "balance " + to_string(balance_); }

    private:
        int balance_;
    };

    class savings_account final : public account {
    public:
        savings_account(const int balance, const double rate) : account{balance}, rate_{rate} {}

        string describe() const override { return account::describe() + ", rate " + to_string(rate_); }

    private:
        double rate_;
    };

    // class premium_account : public savings_account {};  // compiler error: `savings_account` is `final`

    /* --- `extend_the_base_version` ---
     * And `savings_account` is `final`: no class can be derived from it. That documents a decision - and helps the
     * compiler to call its functions directly (see the session).
     * - !![#override]
     */
    void extend_the_base_version() {
        print_function_header();

        const savings_account s{100, 0.02};
        const account& a{s};
        cout << " 1| " << a.describe() << '\n';
    }

    /* --- `rectangle` and `square` ---
     * Mathematics says: a square is a rectangle. So `square` derives from `rectangle` - and has to keep its sides
     * equal: `set_width` changes the height, too. `set_width` is virtual for that.
     */
    class rectangle {
    public:
        rectangle(const int width, const int height) : width_{width}, height_{height} {}
        virtual ~rectangle() = default;

        virtual void set_width(const int width) { width_ = width; }
        int width() const { return width_; }
        int area() const { return width_ * height_; }

    protected:
        int width_;
        int height_;
    };

    class square : public rectangle {
    public:
        explicit square(const int side) : rectangle{side, side} {}

        void set_width(const int width) override { width_ = height_ = width; }
    };

    /* --- `double_the_width` --- Promises to double the area of a rectangle. */
    void double_the_width(rectangle& r) {
        const int before{r.area()};
        r.set_width(2 * r.width());
        cout << " a|   area before " << before << ", after " << r.area() << '\n';
    }

    /* --- `choose_is_a_or_has_a` ---
     * `double_the_width` works for every rectangle - a 2 x 3 becomes a 4 x 3, the area doubles. For a square, the
     * height doubles, too, and the area is four times as big. The function relies on something that is true for
     * rectangles and false for squares: that the width can change on its own.
     * The rule of Barbara Liskov: an object of a derived class must be usable wherever an object of the base class is
     * expected - with everything the base promises. A square cannot keep the promises of this rectangle, so it should
     * not derive from it. "Is a" in the sense of inheritance means "behaves like a", not "is called a" in mathematics.
     * If a class only needs what another one does, it has one - as a member. Liskov's rule is the L of SOLID, five
     * principles for designing classes; four of them are about interfaces and inheritance.
     * - !![#inheritance]
     * - !![#solid]
     */
    void choose_is_a_or_has_a() {
        print_function_header();

        rectangle r{2, 3};
        square q{3};
        cout << " 1| rectangle 2 x 3\n";
        double_the_width(r);
        cout << " 2| square 3 x 3\n";
        double_the_width(q);
    }

}

/* --- `main` --- */
int main() {
    use_access();
    inherit_constructors();
    bring_back_hidden_overloads();
    extend_the_base_version();
    choose_is_a_or_has_a();

    return EXIT_SUCCESS;
}
