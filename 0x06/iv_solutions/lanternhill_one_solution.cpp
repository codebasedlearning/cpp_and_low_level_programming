// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

// One solution for task 'Lantern Hill', see ../tasks.md.

#include <iostream>
#include <string>
#include <memory>
#include <utility>
#include <cstdlib>
#include <cbl/heap_watch.hpp>               // in your project: a copy of `heap_watch.hpp`

using std::cout, std::string, std::unique_ptr, std::make_unique, std::shared_ptr, std::make_shared;

class tracer {
public:
    explicit tracer(const string& name) : name_{name} {
        cout << "ctor " << name_ << '\n';
    }

    ~tracer() {
        cout << "dtor " << name_ << '\n';
    }

    const string& name() const { return name_; }

private:
    string name_;
};

unique_ptr<tracer> make(const string& name) {
    return make_unique<tracer>(name);
}

void take(const unique_ptr<tracer> t) {
    cout << "take " << t->name() << '\n';
}

/*
 * The output, with the reason:
 *
 *   ctor a             make("a")           - one `tracer`, built on the heap, returned as its owner
 *   a is empty         std::move(a)        - `b` took the ownership, `a` holds `nullptr`
 *   take a                                 - the parameter `t` owns it now
 *   dtor a                                 - `t` dies when `take` is done, and deletes it
 *   after take
 *   ctor s             make_shared         - count 1
 *   count 2                                - `t` is a second owner
 *   count 1            s.reset()           - `s` gives up, `t` still owns it
 *   dtor s             }                   - the last owner dies
 *   end of main                            - `a`, `b` and `s` are empty, nothing is left to destroy
 *
 * Allocations: 2 - the `tracer` "a", and one block for "s" and its control block. The names are short: no block
 * for the `string`s (SSO).
 */
int main() {
    const heap_watch heap{};
    {
        unique_ptr<tracer> a{make("a")};
        unique_ptr<tracer> b{std::move(a)};
        cout << "a is " << (a ? "full" : "empty") << '\n';
        take(std::move(b));
        cout << "after take\n";
        shared_ptr<tracer> s{make_shared<tracer>("s")};
        {
            const shared_ptr<tracer> t{s};
            cout << "count " << s.use_count() << '\n';
            s.reset();
            cout << "count " << t.use_count() << '\n';
        }
        cout << "end of main\n";
    }
    cout << "allocations: " << heap.allocations() << '\n';

    // Extension: with `shared_ptr<tracer>{new tracer{"s"}}`, the output is the same - but 3 allocations: the `tracer`
    // and its control block are two blocks now.
    // Extension: `shared_ptr<tracer> d{make("d")};` compiles - a `shared_ptr` can take over from a `unique_ptr` that is
    // about to die. 2 allocations: the `tracer`, then the control block, which `make` did not create.
    const heap_watch extension{};
    {
        const shared_ptr<tracer> d{make("d")};
        cout << "d.use_count()=" << d.use_count() << '\n';
    }
    cout << "allocations: " << extension.allocations() << '\n';

    return EXIT_SUCCESS;
}
