// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

// One solution for task 'Little Harbor', see ../tasks.md.

#include <iostream>
#include <string>
#include <cstdlib>

using std::cout, std::string;

int main() {
    cout << "sizeof(string)=" << sizeof(string) << '\n';

    string s{};
    for (int i{0}; i < 32; ++i) {
        const void* chars{s.c_str()};       // as `const void*`, `cout` prints the address
        cout << "length=" << s.size() << ", capacity=" << s.capacity()
             << ", object at " << &s << ", characters at " << chars << '\n';
        s += 'x';
    }

    // The object stays where it is. The characters lie inside it (a few bytes above `&s`)
    // up to a threshold, then they jump to the heap - at the same moment the capacity grows.
    // gcc (libstdc++): up to 15 characters inside the object, clang (libc++): up to 22.

    return EXIT_SUCCESS;
}
