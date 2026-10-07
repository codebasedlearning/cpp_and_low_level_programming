// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

// One solution for task 'Ember Falls', see ../tasks.md.

#include <iostream>
#include <memory>
#include <cstdlib>

using std::cout, std::unique_ptr, std::make_unique;

class ticket {
public:
    ticket() : number_{next_} {
        ++next_;
        ++live_;
    }

    ~ticket() {
        --live_;
    }

    ticket(const ticket&) = delete;
    ticket& operator=(const ticket&) = delete;

    int number() const { return number_; }

    static int live() { return live_; }
    static int issued() { return next_ - 1; }

private:
    int number_;
    static int next_;
    static int live_;
};

// The definitions. Split into header and source, they go into `ticket.cpp` - in exactly one file. In the header, the
// linker reports `multiple definition of ticket::next_` as soon as two `.cpp` files include it. With
// `inline static int next_{1};` in the class, these two lines go away, and the header is fine.
// `nm -C ticket.o`: `ticket::next_` gets a `D` - it starts at 1, so its value must be in the file - and
// `ticket::live_` a `B`, the part of static storage that is set to zero at the start and takes no space in the file.
int ticket::next_{1};
int ticket::live_{0};

// Extension: a `static` local ticket - created at the first call, not before `main`. It gets the number that is next
// at that moment.
const ticket& first_ticket() {
    static const ticket first;
    return first;
}

void print_counts(const char* when) {
    cout << when << ": live=" << ticket::live() << ", issued=" << ticket::issued() << '\n';
}

int main() {
    // 4 bytes: the `int` number. The counters are not in the object.
    cout << "sizeof(ticket) = " << sizeof(ticket) << '\n';

    const ticket a;
    print_counts("a");
    {
        const ticket b;
        const ticket c;
        print_counts("b and c");
        {
            const unique_ptr<ticket> d{make_unique<ticket>()};
            cout << "d has number " << d->number() << '\n';
            print_counts("d on the heap");
        }
        print_counts("d gone");
    }
    print_counts("b and c gone");

    cout << "first_ticket has number " << first_ticket().number() << '\n';
    print_counts("after first_ticket");
    cout << "a has number " << a.number() << '\n';

    return EXIT_SUCCESS;
}
