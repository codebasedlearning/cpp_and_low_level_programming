// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

// One solution for task 'Don't do this, or I'll tell your mum', see ../tasks.md.
// Undefined behavior on purpose - the output depends on the compiler, the options and the platform.

#include <iostream>
#include <cstdlib>

using std::cout, std::endl, std::hex, std::dec;

void change_the_neighbors() {
    int a{0x11111111};
    int b{0x22222222};
    int c{0x33333333};
    cout << "&a=" << &a << ", &b=" << &b << ", &c=" << &c << endl;

    int* const p{&b};
    *(p + 1) = 0x44;                        // undefined behavior: `p + 1` is one past `b` - no object of ours
    *(p - 1) = 0x55;                        // undefined behavior: already computing `p - 1` is
    cout << hex << "a=" << a << ", b=" << b << ", c=" << c << dec << endl;
}

void write_past_an_array() {
    int before{1};
    int values[3]{10, 20, 30};
    int after{2};
    cout << "&before=" << &before << ", &values[3]=" << &values[3] << ", &after=" << &after << endl;

    int* const past_the_end{values + 3};
    *past_the_end = 99;                     // undefined behavior
    cout << "before=" << before << ", after=" << after << endl;
}

int main() {
    change_the_neighbors();
    write_past_an_array();

    // What we saw on x86-64 Linux, gcc 13 and clang 18 (the frames differ from program to program, so yours will, too):
    // - gcc placed `a`, `b`, `c` upwards, clang downwards - so `p + 1` hit `c` with gcc and `a` with clang (Debug).
    // - clang as Release: `p + 1` was the address of `c`, and still it printed `c=33333333`. The compiler assumes that
    //   nothing writes to `c` behind its back, and prints the value it knows.
    // - Writing past the array: clang (Debug) overwrote `before`. gcc (Debug) hit the guard value of the stack
    //   protector, and the program ended with `*** stack smashing detected ***` (exit status 134): on Ubuntu, gcc
    //   builds with `-fstack-protector-strong`, which puts a random value behind the arrays of a frame and checks it
    //   before the function returns. The other builds showed nothing at all.
    // - gcc warns about all three writes as Release (`-Warray-bounds`) - it sees them, too.
    // - AddressSanitizer reports the first bad write, with the variable and the line, and stops.

    return EXIT_SUCCESS;
}
