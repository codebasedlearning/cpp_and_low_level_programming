// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

// One solution for task 'Beverly Hollow', see ../tasks.md.

#include <iostream>
#include <cstdint>
#include <cstdlib>

using std::cout, std::uint64_t;

int main() {
    // input value - console input comes in the next unit
    const int n{20};

    // f_0 = 0, f_1 = 1, f_i = f_(i-1) + f_(i-2)
    uint64_t f_prev{0};                     // f_(i-1)
    uint64_t f_curr{1};                     // f_i
    cout << "f_0=" << f_prev << '\n';
    for (int i{1}; i < n; ++i) {
        cout << "f_" << i << "=" << f_curr << '\n';
        const uint64_t f_next{f_prev + f_curr};
        f_prev = f_curr;
        f_curr = f_next;
    }

    // `int` is enough up to f_46 = 1836311903; f_47 = 2971215073 is larger than 2^31-1.
    // `uint64_t` is enough up to f_93 = 12200160415121876738; f_94 no longer fits into 64 bits.
    //
    // Extension: the recursive `fib` in `d_functions` needs 2,692,537 calls for fib(30), each with its own stack frame.
    // The loop needs 30 steps - that is the difference, not "recursion is slow" in general.

    return EXIT_SUCCESS;
}
