// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - One keyword, three jobs. At namespace scope, `static` hides a name in its file - today: the unnamed namespace. In a
 *   function, it makes a local variable live as long as the program. In a class, it makes a member belong to the
 *   class.
 * - A static data member is declared in the class and defined once: in exactly one `.cpp` file - or with `inline` in
 *   the class. `static constexpr` members are `inline` anyway.
 * - A static member function can call a private constructor: a named constructor.
 * - Counting instances - the copies, too.
 * - A table in a function-local static: built at the first call, shared by all later ones.
 * - The order of initialization across files is not defined.
 */

#include <iostream>
#include <vector>
#include <cstdlib>
#include <cbl/printing.hpp>

using std::cout, std::vector;


/* ---- Content ---- */

/* --- `calls_in_this_file` ---
 * `static` at namespace scope: the variable is visible in this file only - internal linkage, a lowercase letter in
 * `nm`. The unnamed namespace does the same, for everything in it, types included - that is why this course puts its
 * functions there (see previous snippets). `static` for this job is the C way, and still common.
 */
static int calls_in_this_file{0};

namespace {

    /* --- `thermometer` ---
     * `count_` is declared in the class - `static int count_;` - and defined outside, below. In a class split into
     * header and source, the declaration is in the header and the definition in the `.cpp` file. `readings_` is the
     * newer way: `inline static`, defined right in the class (C++17). `absolute_zero` is `static constexpr`, which is
     * `inline` by itself.
     * - !![#static-member]
     */
    class thermometer {
    public:
        thermometer() { ++count_; }
        ~thermometer() { --count_; }
        thermometer(const thermometer&) = delete;
        thermometer& operator=(const thermometer&) = delete;

        void read(const double celsius) {
            ++readings_;
            last_ = celsius < absolute_zero ? absolute_zero : celsius;
        }

        double last() const { return last_; }
        static int count() { return count_; }
        static int readings() { return readings_; }

        static constexpr double absolute_zero{-273.15};

    private:
        double last_{0.0};
        static int count_;
        inline static int readings_{0};
    };

    /* --- `thermometer::count_` --- The definition - in a header, this line would be wrong, see the Q&A below. */
    int thermometer::count_{0};

    /* --- `define_a_static_member` --- Two thermometers, one count, one number of readings. */
    void define_a_static_member() {
        print_function_header();

        ++calls_in_this_file;
        thermometer inside;
        thermometer outside;
        inside.read(21.5);
        outside.read(-300.0);
        cout << " 1| count=" << thermometer::count() << ", readings=" << thermometer::readings() << ", outside="
             << outside.last() << '\n';

        /* -- .Q&A -- !![Why must a static member without `inline` be defined in exactly one `.cpp` file?](#a-712) */
    }

    /* --- `temperature` ---
     * The constructor is private, and takes kelvin. Two static member functions - named constructors - convert and
     * call it: `temperature::from_celsius(20.0)` says what the number means, `temperature{20.0}` would not. A static
     * member function belongs to the class, so it may use the private constructor.
     * - !![#named-constructor]
     */
    class temperature {
    public:
        static temperature from_celsius(const double celsius) { return temperature{celsius + 273.15}; }
        static temperature from_fahrenheit(const double fahrenheit) {
            return temperature{(fahrenheit - 32.0) * 5.0 / 9.0 + 273.15};
        }

        double kelvin() const { return kelvin_; }

    private:
        explicit temperature(const double kelvin) : kelvin_{kelvin} {}
        double kelvin_;
    };

    /* --- `use_a_named_constructor` --- Two ways in, one representation. */
    void use_a_named_constructor() {
        print_function_header();

        ++calls_in_this_file;
        const temperature room{temperature::from_celsius(20.0)};
        const temperature same{temperature::from_fahrenheit(68.0)};
        cout << " 1| room=" << room.kelvin() << " K, same=" << same.kelvin() << " K\n";
        // const temperature t{20.0};       // compiler error: the constructor is private
    }

    /* --- `instance` ---
     * Counts its objects - the copies included: the copy constructor counts, too. `alive` is how many exist now,
     * `created` how many were ever made.
     */
    class instance {
    public:
        instance() { count_up(); }
        instance(const instance&) { count_up(); }
        instance& operator=(const instance&) = default;
        ~instance() { --alive; }

        inline static int alive{0};
        inline static int created{0};

    private:
        static void count_up() {
            ++alive;
            ++created;
        }
    };

    /* --- `pass_by_value` --- A copy - and a count. */
    void pass_by_value(const instance) {
        cout << " a|   inside: alive=" << instance::alive << '\n';
    }

    /* --- `count_instances` --- A parameter by value is an object, too - and so is every element of a `vector`. */
    void count_instances() {
        print_function_header();

        ++calls_in_this_file;
        const instance a;
        pass_by_value(a);
        cout << " 1| alive=" << instance::alive << ", created=" << instance::created << '\n';
        {
            const vector<instance> three(3);
            cout << " 2| alive=" << instance::alive << ", created=" << instance::created << '\n';
        }
        cout << " 3| alive=" << instance::alive << ", created=" << instance::created << '\n';
    }

    /* --- `make_factorials` --- Computes 0! to 20! - the largest that fit into a `long long`. */
    vector<long long> make_factorials() {
        cout << " b|   building the table\n";
        vector<long long> table{1};
        for (long long n{1}; n <= 20; ++n) {
            table.push_back(table.back() * n);
        }
        return table;
    }

    /* --- `factorials` ---
     * The table is built at the first call, and every later call returns a reference to the same one. A function that
     * returns a reference to its static local is also the simplest thread-safe singleton - one object for the whole
     * program, created when it is first needed.
     */
    const vector<long long>& factorials() {
        static const vector<long long> table{make_factorials()};
        return table;
    }

    /* --- `cache_in_a_local_static` --- Three calls, one table. */
    void cache_in_a_local_static() {
        print_function_header();

        ++calls_in_this_file;
        const long long five{factorials()[5]};
        cout << " 1| 5!=" << five << '\n';
        cout << " 2| 10!=" << factorials()[10] << ", 20!=" << factorials()[20] << '\n';
        cout << " 3| calls_in_this_file=" << calls_in_this_file << '\n';
    }

}

/* --- The initialization order fiasco ---
 * Global variables and static data members are initialized before `main`. Within one file, from top to bottom. Across
 * files, in an order the standard does not fix: if a global in `a.cpp` uses a global from `b.cpp` in its initializer,
 * it may see that one before it is initialized - as zeros. It depends on the order in which the linker got the files.
 * Two ways out: a function-local static, as `factorials`, is initialized at its first use, whatever the order of the
 * files; and `constinit` (C++20) insists on an initialization at compile time - it is a compiler error if that is not
 * possible.
 * - !![#static-storage]
 */

/* --- `main` --- */
int main() {
    define_a_static_member();
    use_a_named_constructor();
    count_instances();
    cache_in_a_local_static();

    return EXIT_SUCCESS;
}
