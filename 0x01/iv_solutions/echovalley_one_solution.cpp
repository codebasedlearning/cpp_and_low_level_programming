// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

// One solution for task 'Echo Valley', see ../tasks.md.

#include <iostream>
#include <cstdlib>

using std::cout;

#pragma GCC diagnostic ignored "-Wuninitialized"    // we read `value` on purpose

void set_value() {
    int value{4711};
    cout << "set_value:  value=" << value << '\n';
}

void read_value() {
    int value;                              // no value - reading it is undefined behavior
    cout << "read_value: value=" << value << '\n';
}

int main() {
    set_value();
    read_value();

    // Debug (-O0): `read_value` usually prints 4711. Its stack frame lies where the frame of
    // `set_value` was, and `value` gets the same slot - released, but not cleared.
    // Release (-O2): usually something else, e.g. 0. The optimizer keeps `value` in a register
    // or drops it, because reading an uninitialized variable is UB and may be assumed not to happen.
    //
    // Extension: with a second variable in `set_value`, the slots can shift - whether
    // `read_value` still "finds" 4711 depends on the compiler's layout of both frames.

    return EXIT_SUCCESS;
}
