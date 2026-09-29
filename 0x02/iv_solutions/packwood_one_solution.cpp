// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

// One solution for task 'Packwood', see ../tasks.md.

#include <iostream>
#include <cstddef>
#include <cstdlib>

using std::cout;

struct account {                            // the order from the task
    bool active;                            //  0     - then 7 bytes padding for `balance`
    double balance;                         //  8..15
    int id;                                 // 16..19
    char grade;                             // 20     - then 3 bytes padding for `limit`
    double limit;                           // 24..31
};

struct account_packed {                     // largest alignment first
    double balance;                         //  0..7
    double limit;                           //  8..15
    int id;                                 // 16..19
    bool active;                            // 20
    char grade;                             // 21     - then 2 bytes padding, see below
};

int main() {
    cout << "account:        sizeof=" << sizeof(account) << ", offsets active=" << offsetof(account, active)
         << ", balance=" << offsetof(account, balance) << ", id=" << offsetof(account, id)
         << ", grade=" << offsetof(account, grade) << ", limit=" << offsetof(account, limit) << '\n';
    cout << "account_packed: sizeof=" << sizeof(account_packed) << '\n';

    // 22 bytes of data, but 24 in total: in an array the next `balance` must be aligned to 8, so the struct size is
    // rounded up to a multiple of its largest alignment.
    // Extension: 10'000'000 x (32 - 24) bytes = 80 MB saved.

    return EXIT_SUCCESS;
}
