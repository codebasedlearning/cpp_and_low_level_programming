// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

// One solution for task 'McAllen Spring', see ../tasks.md.

#include <iostream>
#include <cstdlib>

using std::cout;

class node {
public:
    explicit node(const int payload, node* next = nullptr) : payload_{payload}, next_{next} {}

    int payload() const { return payload_; }
    node* next() const { return next_; }

    void where() const {
        cout << "  this=" << this << ", payload=" << payload_ << ", next=" << next_ << '\n';
    }

private:
    int payload_;
    node* next_;
};

// Extension: the length, with a loop and recursively.
int length(const node* first) {
    int count{0};
    for (const node* p{first}; p != nullptr; p = p->next()) {
        ++count;
    }
    return count;
}

int length_recursive(const node* first) {
    return first == nullptr ? 0 : 1 + length_recursive(first->next());
}

// Extension: the classic mistake - the node lives in the frame of `make_node` and dies at its `}`. gcc and clang warn
// ("address of local variable returned"), gcc even returns a null pointer instead. So it stays a comment here.
// node* make_node(const int payload, node* next) {
//     node n{payload, next};
//     return &n;
// }

int main() {
    node d{7};
    node c{5, &d};
    node b{3, &c};
    node head{2, &b};

    cout << "payloads:";
    for (const node* p{&head}; p != nullptr; p = p->next()) {
        cout << ' ' << p->payload();
    }
    cout << '\n';

    // Extension: an `int` (4 bytes), 4 bytes of padding, a pointer (8 bytes) - 16 bytes on a 64-bit platform.
    cout << "sizeof(node)=" << sizeof(node) << '\n';

    // Extension: the nodes are local variables, so they are in the frame of `main` - here 16 bytes apart, in an order
    // the compiler chooses. Each `next` is the `this` of another node.
    for (const node* p{&head}; p != nullptr; p = p->next()) {
        p->where();
    }

    cout << "length=" << length(&head) << ", recursively=" << length_recursive(&head) << '\n';

    return EXIT_SUCCESS;
}
