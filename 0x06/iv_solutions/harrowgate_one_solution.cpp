// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

// One solution for task 'Harrow Gate', see ../tasks.md.

#include <iostream>
#include <vector>
#include <utility>
#include <cstring>
#include <cstddef>
#include <cstdlib>
#include <cbl/heap_watch.hpp>               // in your project: a copy of `heap_watch.hpp`

using std::cout, std::vector, std::strlen, std::memcpy, std::size_t;

class text {
public:
    explicit text(const char* chars) : chars_{new char[strlen(chars) + 1]}, size_{strlen(chars)} {
        memcpy(chars_, chars, size_ + 1);   // with the '\0'
    }

    ~text() { delete[] chars_; }

    text(const text& other) : chars_{new char[other.size_ + 1]}, size_{other.size_} {
        memcpy(chars_, other.chars_, size_ + 1);
    }

    // First the new block, then the old one goes: if `new` throws, nothing has changed, and `a = a` works.
    text& operator=(const text& other) {
        char* block{new char[other.size_ + 1]};
        memcpy(block, other.chars_, other.size_ + 1);
        delete[] chars_;
        chars_ = block;
        size_ = other.size_;
        return *this;
    }

    // A moved-from `text` has no block. Its destructor does nothing, and `c_str()` returns "" - so it stays usable.
    text(text&& other) noexcept : chars_{other.chars_}, size_{other.size_} {
        other.chars_ = nullptr;
        other.size_ = 0;
    }

    text& operator=(text&& other) noexcept {
        if (this != &other) {
            delete[] chars_;
            chars_ = other.chars_;
            size_ = other.size_;
            other.chars_ = nullptr;
            other.size_ = 0;
        }
        return *this;
    }

    size_t size() const { return size_; }
    const char* c_str() const { return chars_ != nullptr ? chars_ : ""; }

private:
    char* chars_;
    size_t size_;
};

// Prints the line and the allocations since the last call.
void print_allocations(heap_watch& heap, const char* line) {
    cout << line << "  // " << heap.allocations() << " allocation(s)\n";
    heap.reset();
}

int main() {
    heap_watch heap{};

    text a{"Blue in Green"};
    print_allocations(heap, "text a{\"Blue in Green\"};");     // 1: the block
    text b{a};
    print_allocations(heap, "text b{a};");                    // 1: a copy has a block of its own
    text c{std::move(a)};
    print_allocations(heap, "text c{std::move(a)};");         // 0: c takes a's block
    b = c;
    print_allocations(heap, "b = c;");                        // 1: a new block for b, the old one is released
    c = text{"So What"};
    print_allocations(heap, "c = text{\"So What\"};");         // 1: the temporary's block - moved into c
    vector<text> v;
    v.push_back(b);
    print_allocations(heap, "v.push_back(b);");               // 2: the vector's block, and a copy of b
    v.push_back(std::move(c));
    // 1: a bigger block for the vector - the copy of b is moved over, c is moved in
    print_allocations(heap, "v.push_back(std::move(c));");
    cout << "v[0]=" << v[0].c_str() << ", v[1]=" << v[1].c_str() << ", c=\"" << c.c_str() << "\"\n";

    // Extension: without `noexcept` on the move constructor, the vector copies its elements when it grows - one
    // allocation for each element that is already there. Five `push_back(text{...})`s: with `noexcept` 9 allocations
    // (5 texts, 4 blocks for the vector: capacity 1, 2, 4, 8), without 16 - the copies when it grows to 2, 4 and 8:
    // 1 + 2 + 4 more.
    //
    // Extension: with `std::unique_ptr<char[]> chars_`, the destructor can go. The copy constructor and the copy
    // assignment must stay: `unique_ptr` cannot be copied, so the generated copies are deleted. The generated moves
    // would work, but leave `size_` in the moved-from object as it was (as the `smart_buffer` of the session) - so
    // either keep the hand-written moves, or accept that a moved-from `text` may only be destroyed or assigned to.

    return EXIT_SUCCESS;
}
