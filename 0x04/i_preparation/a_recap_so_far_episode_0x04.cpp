// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - The best of units 0x01-0x03 - the things to remember.
 */

#include <iostream>
#include <string>
#include <stdexcept>
#include <cstdlib>
#include <map>
#include <set>
#include <vector>
#include <cbl/printing.hpp>

using std::cout, std::dec, std::hex, std::showbase;
using std::string, std::runtime_error;
using std::array, std::vector, std::set, std::map;

/* ---- Content ---- */

namespace {

    /*--- recall_init_vars_on_stack ---
     * - uniform init
     * - memory address
     * - sizeof
     */
    void recall_init_vars_on_stack() {
        print_function_header();

        const int n{0x00112233};
        cout << " 1| n=" << showbase << hex << n <<" at " << &n << ", " << dec << sizeof(n) << " bytes\n";
    }

    /*--- recall_pointers_and_refs ---
     * - pointer
     * - deref a pointer with *
     * - reference
     */
    void recall_pointers_and_refs() {
        print_function_header();

        int n{23};
        cout << " 1| n=" << n <<" at " << &n << "\n";

        int& rn{n};
        rn = 42;
        cout << " 2| rn=" << rn << " at " << &rn << ", n=" << n << "\n";

        int* pn{&n};
        cout << " 3| pn=" << pn << " at " << &pn << ", *pn=" << *pn << " at " << &(*pn) << "\n";
    }

    /*--- recall_collections ---
     * - array
     * - vector
     * - set, unordered_set
     * - map, unordered_map
     */
    void recall_collections() {
        print_function_header();

        const std::array a{1, 2, 3};
        cout << " 1| a[0]=" << a[0] << ", a[1]=" << a[1] << ", a[2]=" << a[2] << "\n";

        const std::vector v{11, 22, 34};
        cout << " 2| v[0]=" << v[0] << ", v[1]=" << v[1] << ", v[2]=" << v[2] << "\n";

        const std::vector w(3, 42);
        cout << " 3| w[0]=" << w[0] << ", w[1]=" << w[1] << ", w[2]=" << w[2] << "\n";

        const std::set s{1,2,2,3,3,3};
        cout << " 4| s.size=" << s.size() << ", 1 in s:" << s.contains(1) << "\n";

        const std::map<int,int> m{{1,2},{3,4}};
        cout << " 5| m.size=" << m.size() << ", m[3]=" << m.at(3) << "\n";
    }

    /*--- recall_collections ---
     * - range-based for
     * - if with init
     */
    void recall_control_structures() {
        print_function_header();

        const std::vector words{"these","are","words"};
        cout << " 1| words=[ ";
        for (const auto& word : words)
            cout << "'" << word << "' ";
        cout << "]\n";

        cout << " 1| words empty? ";
        if (const auto sz=words.size(); sz==0) {
            cout << "yes";
        } else {
            cout << "no (size=" << words.size() << ")";
        }
        cout << "\n";
    }

    unsigned long length(const std::vector<int>& v);

    /*--- recall_functions_and_call_by_ref_vs_value ---
     * - declaration and definition
     */
    void recall_functions_and_call_by_ref_vs_value() {
        print_function_header();

        const std::vector v(1000000, 42);
        cout << " 1| length(v)=" << length(v) << "\n";
    }

    unsigned long length(const std::vector<int>& v) {
        return v.size();
    }

    /*--- .`fraction` ---
     * An object is its data members, plus padding - nothing else. The constructor initializes them in the member
     * initializer list, in declaration order.
     */
    struct fraction {
        int num;
        int denom = 1;
    };

    /*--- .operator<< ---
     * - shift op<<
     */
    std::ostream& operator<<(std::ostream& os, const fraction& frac) {
        return os << frac.num << "/" << frac.denom;
    }

    /*--- recall_structs ---
     * - all public
     */
    void recall_structs() {
        print_function_header();

        const fraction f1{};
        cout << " 1| f1=" << f1 << "\n";
        const fraction f2{2,3};
        cout << " 2| f2=" << f2 << "\n";
        const fraction f3{.num=4,.denom=5};
        cout << " 3| f3=" << f3 << "\n";
        const auto f4 = f3;
        cout << " 4| f4=" << f4 << "\n";
    }

    /* --- `recall_objects_are_bytes` ---
     * - Two `int`s. sizeof() is sum of members + padding.
     * - The member functions exist once in the program, not in every object.
     */
    void recall_objects_are_bytes() {
        print_function_header();

        const fraction f{0x11223344, 0x55667788};
        cout << " 1| sizeof(f)=" << sizeof(f) << ", &f=" << &f << '\n';
    }

    /*--- .`tracer` ---
     * - Reports its birth, its copies and its death.
     * - default
     * - deleted
     */
    class tracer {
    public:
        // tracer() = default;
         explicit tracer(const string& name) : name_{name} {
            cout << " a|   " << name_ << ": constructed\n";
        }

        tracer(const tracer& other) : name_{other.name_ + "'"} {
            cout << " b|   " << name_ << ": copy-constructed\n";
        }

        tracer& operator=(const tracer& other) {
            cout << " c|   " << name_ << ": assigned from " << other.name_ << '\n';
            name_ = other.name_ + "''";
            return *this;
        }

        string name() const { return name_; }

        void clear() { name_ = "-"; }

        ~tracer() {
            cout << " d|   " << name_ << ": destroyed\n";
        }

    private:
        string name_;
    };

    /*--- .`tracer_pair` ---
     * - member management
     */
    class tracer_pair {
        public:
        explicit tracer_pair(const string& name) : tracer1_{name+"1"},tracer2_ {name+"2"} {
            cout << " e|   " << name << ": constructed\n";
            throw std::runtime_error{"invalid"};
        }
    private:
        tracer tracer1_;
        tracer tracer2_;
    };

    /* --- `recall_copy_or_assign` ---
     * - A copy construction creates a new object.
     * - A copy assignment overwrites an existing one.
     * - The destructors run at the `}`, in reverse order of construction.
     */
    void recall_copy_or_assign() {
        print_function_header();

        const tracer t1{"t1"};
        tracer t2{t1};
        tracer t3{"t3"};
        t2 = t3;
        // t2 = string("?");

        // t1.clear();
        t3.clear();
        cout << " 1| t1=" << t1.name() << ", t2=" << t2.name() << ", t3=" << t3.name() << "\n";
    }

    /* --- `recall_stack_unwinding` ---
     * - On the way to the `catch` or at return, every frame is removed, and every local object in it is destroyed
     * - RAII: the cleanup is in the destructor, and it cannot be forgotten.
     */
    void recall_RAII() {
        print_function_header();

        try {
            tracer_pair("check exception");
        } catch (const runtime_error& e) {
            cout << " 1| caught: " << e.what() << '\n';
        }

        if (true) {
            tracer t("check return");
            return;
        }
    }

    /* --- `recall_a_lot_of_consts` ---
     * - const vars, refs
     * - const members
     */
    void recall_a_lot_of_consts() {
        print_function_header();

        tracer t1{"t1"};
        t1.clear();

        const tracer& t2{t1};
        // t2.clear();
        auto s{t2.name()};
    }
}

/* --- Header and source ---
 * A class in real projects: the header declares it, including its data members - every translation unit that creates
 * an object needs its `sizeof`. The source file defines the member functions, the linker joins the object files.
 * - A missing definition: `undefined reference` (gcc), `Undefined symbols` (Apple clang).
 * - A function body in a header that is included twice: `multiple definition`, `duplicate symbol`. Fix: move it into
 *   the `.cpp` file, or mark it `inline`.
 */

/* --- Toolbox ---
 * What you can use by now to look at the machine:
 * - Debug (`-O0`) vs. Release (`-O2`), `g++ -S` or Compiler Explorer (godbolt.org).
 * - The debugger: breakpoints, the memory view, stepping (into, over, out) and the call stack.
 * - `sizeof`, `alignof`, `offsetof` and printed addresses.
 * - `stopwatch` - measure, as Release, instead of guessing.
 */

/* --- Teaser ---
 * The linker looks for `fraction::fraction(int, int)` - by name. But `print(int)` and `print(double)` have the same
 * name, and so do a hundred member functions called `size`. Under which name does the linker look for a function?
 */

/* --- `main` --- */
int main() {
    recall_init_vars_on_stack();
    recall_pointers_and_refs();
    recall_collections();
    recall_control_structures();
    recall_functions_and_call_by_ref_vs_value();
    recall_structs();
    recall_objects_are_bytes();
    recall_copy_or_assign();
    recall_RAII();
    recall_a_lot_of_consts();

    return EXIT_SUCCESS;
}
