// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

// One solution for task 'Harshire', see ../tasks.md.

#include <iostream>
#include <array>
#include <stdexcept>
#include <cstdlib>

using std::cout, std::array, std::runtime_error;

struct stack {
    array<int, 3> data{};
    size_t next{0};                         // next free position = number of elements
};

// By reference - both functions change the stack of the caller.
void push(stack& s, const int value) {
    if (s.next >= s.data.size()) {
        throw runtime_error{"stack is full"};
    }
    s.data[s.next] = value;
    ++s.next;
}

int pop(stack& s) {
    if (s.next == 0) {
        throw runtime_error{"stack is empty"};
    }
    --s.next;
    return s.data[s.next];
}

int main() {
    stack s{};

    try {
        for (const int value : {2, 3, 5, 7}) {
            push(s, value);
            cout << "pushed " << value << ", size=" << s.next << '\n';
        }
    } catch (const runtime_error& e) {
        cout << "error: " << e.what() << '\n';
    }

    try {
        for (int i{0}; i < 4; ++i) {
            const int value{pop(s)};
            cout << "popped " << value << ", size=" << s.next << '\n';
        }
    } catch (const runtime_error& e) {
        cout << "error: " << e.what() << '\n';
    }

    return EXIT_SUCCESS;
}
