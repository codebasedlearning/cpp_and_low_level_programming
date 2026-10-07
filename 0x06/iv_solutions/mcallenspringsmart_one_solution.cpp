// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

// One solution for task 'McAllen Spring Smart', see ../tasks.md.

#include <iostream>
#include <memory>
#include <utility>
#include <cstdlib>

using std::cout, std::endl, std::unique_ptr, std::make_unique;

// Set to false for the extension with a million nodes - a million lines are no fun.
constexpr bool verbose{true};

class node {
public:
    node(const int payload, unique_ptr<node> next) : payload_{payload}, next_{std::move(next)} {}

    ~node() {
        if (verbose) {
            cout << "  dtor " << payload_ << '\n';
        }
    }

    int payload() const { return payload_; }
    const node* next() const { return next_.get(); }

    // For `clear`: hands over the rest of the list.
    unique_ptr<node> take_next() { return std::move(next_); }

private:
    int payload_;
    unique_ptr<node> next_;
};

void push_front(unique_ptr<node>& head, const int payload) {
    head = make_unique<node>(payload, std::move(head));
}

// Extension: one node after the other. The next node is taken away before the head dies, so each destructor has
// nothing left to delete - no recursion.
void clear(unique_ptr<node>& head) {
    while (head != nullptr) {
        head = head->take_next();           // the old head dies here, alone
    }
}

int main() {
    unique_ptr<node> head;
    for (const int payload : {7, 5, 3, 2}) {
        push_front(head, payload);
    }

    cout << "payloads:";
    for (const node* p{head.get()}; p != nullptr; p = p->next()) {
        cout << ' ' << p->payload();
    }
    cout << '\n';

    // The destructor of `head` deletes the first node; its destructor runs, and then its member `next_` is destroyed,
    // which deletes the second node, and so on: 2, 3, 5, 7 - front to back, each call inside the one before.
    cout << "end of the list:\n";
    head.reset();

    // Extension: a million nodes. Without `clear`, the destructors nest a million calls deep - several stack frames per
    // node - and the stack (8 MB on Linux and macOS, 1 MB on Windows) overflows: a crash, exit status 139 on Linux, as
    // Debug and as Release. Set `verbose` to false and remove the `//` in front of `clear` to see both.
    if (!verbose) {
        unique_ptr<node> long_list;
        for (int i{0}; i < 1'000'000; ++i) {
            push_front(long_list, i);
        }
        cout << "a million nodes built" << endl;    // `endl`: flush now, before the crash
        // clear(long_list);
    }
    cout << "end of main\n";

    return EXIT_SUCCESS;
}
