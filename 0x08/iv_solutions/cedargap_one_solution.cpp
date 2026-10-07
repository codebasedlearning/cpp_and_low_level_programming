// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

// One solution for task 'Cedar Gap', see ../tasks.md. The output is in the comments.

#include <iostream>
#include <cstdlib>

using std::cout;

struct base {
    base() { cout << "base() " << who() << '\n'; }
    virtual ~base() { cout << "~base() " << who() << '\n'; }
    void f() const { cout << "base::f\n"; }
    virtual void g() const { cout << "base::g\n"; }
    virtual const char* who() const { return "base"; }
};

struct derived : base {
    derived() { cout << "derived() " << who() << '\n'; }
    ~derived() override { cout << "~derived() " << who() << '\n'; }
    void f() const { cout << "derived::f\n"; }
    void g() const override { cout << "derived::g\n"; }
    const char* who() const override { return "derived"; }
};

// `b.f()`: not virtual - `base::f`, a direct call. `b.g()`: virtual - a call through the table.
void by_reference(const base& b) {
    b.f();
    b.g();
}

// `b` is a copy - a `base`, made by the copy constructor of `base`, with the vptr of `base`. The compiler knows the
// type of a parameter by value: `b.g()` is a direct call to `base::g`.
void by_value(const base b) {
    b.g();
}

int main() {
    cout << "1\n";
    const derived d;            // base() base      - in the constructor of `base`, the object is a `base`
                                // derived() derived
    cout << "2\n";
    const base& r{d};
    r.f();                      // base::f          - the static type decides
    r.g();                      // derived::g       - the dynamic type decides
    cout << "3\n";
    by_reference(d);            // base::f
                                // derived::g
    cout << "4\n";
    by_value(d);                // base::g          - a sliced copy is a `base`
                                // ~base() base     - the parameter is destroyed (no line for its construction: the
                                //                    copy constructor is generated, and prints nothing)
    cout << "5\n";
    const base* p{&d};
    p->base::g();               // base::g          - the class name in front: never virtual
    cout << "6\n";
    const base copy{d};
    copy.g();                   // base::g
    cout << "7\n";
    return EXIT_SUCCESS;        // at the `}`:
}                               // ~base() base     - `copy`, the last one created, is destroyed first
                                // ~derived() derived
                                // ~base() base     - in the destructor of `base`, the object is a `base` again

// Virtual calls in the machine code, `-O0`, x86-64 gcc: only two, each a `call rdx` - `r.g()` in `main`, and `b.g()` in
// `by_reference`. The calls of `who()` in the constructors and destructors are direct: while `base()` runs, the object
// is a `base`, and the compiler knows it. `b.g()` in `by_value`, `p->base::g()` and `copy.g()` are direct, too - the
// type is known, or the call names the class. With `-O2`, gcc sees that `r` refers to `d` and calls `derived::g()`
// directly as well.
// Extension: `sizeof(base)` and `sizeof(derived)` are 8 - the vptr, one for both. An `int` in `derived` makes it 16:
// 8 + 4 + 4 bytes of padding. `base` stays 8.
