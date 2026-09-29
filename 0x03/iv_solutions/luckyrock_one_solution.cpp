// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

// One solution for task 'Lucky Rock', see ../tasks.md.

#include <iostream>
#include <string>
#include <cstdlib>

using std::cout, std::string;

class tracer {
public:
    explicit tracer(const string& name) : name_{name} {
        cout << "ctor " << name_ << '\n';
    }

    tracer(const tracer& other) : name_{other.name_ + "'"} {
        cout << "copy " << name_ << '\n';
    }

    tracer& operator=(const tracer& other) {
        cout << "assign " << name_ << " from " << other.name_ << '\n';
        name_ = other.name_ + "'";
        return *this;
    }

    ~tracer() {
        cout << "dtor " << name_ << '\n';
    }

    const string& name() const { return name_; }

private:
    string name_;
};

tracer make(const string& name) {
    return tracer{name};
}

void show(const tracer t) {
    cout << "show " << t.name() << '\n';
}

/*
 * The output, with the reason:
 *
 *   ctor a             const tracer a{"a"};
 *   copy a'            tracer b = a;  - a construction, not an assignment
 *   ctor c             const tracer c{"c"};
 *   assign a' from c   b = c;         - b exists already
 *   dtor c             }              - c dies at the end of its block
 *   copy a'            show(a);       - the parameter is a copy
 *   show a'
 *   dtor a'                           - the parameter dies at the end of `show`
 *   ctor d             make("d")      - built directly in `d`, no copy
 *   end of main
 *   dtor d             }              - reverse order: d, b, a
 *   dtor c'                           - b, which is a copy of c now
 *   dtor a
 *
 * Extension: with `const tracer&`, the lines `copy a'` and `dtor a'` around `show` disappear, and it prints `show a`.
 */
int main() {
    const tracer a{"a"};
    tracer b = a;
    {
        const tracer c{"c"};
        b = c;
    }
    show(a);
    const tracer d{make("d")};
    cout << "end of main\n";
}
