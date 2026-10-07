// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

#ifndef CBL_HEAP_WATCH_HPP
#define CBL_HEAP_WATCH_HPP

#include <cstddef>                          // size_t
#include <cstdlib>                          // malloc, free
#include <iostream>                         // cout
#include <new>                              // bad_alloc

/*
 * Counts what a program asks from the heap - since it was created, or since the last `reset()`:
 *
 *      heap_watch watch{};
 *      ...                                 // the work to watch
 *      cout << watch.allocations() << " allocations, " << watch.bytes() << " bytes\n";
 *
 * And while a `heap_log` is alive, every allocation and every release is printed:
 *
 *      {
 *          const heap_log log{};
 *          ...                             // prints ' +|   new ...' and ' -|   delete ...'
 *      }
 *
 * How it works: every `new` - your own, and the ones inside `string`, `vector`, `unique_ptr` and all the others - gets
 * its memory from a function called `operator new`, and gives it back with `operator delete`. The standard library has
 * a version of both, and a program may replace them with its own - this header does exactly that. The replacements
 * count, print if asked to, and pass the work on to `malloc` and `free`.
 *
 * Rules that come with it:
 * - Include this header in exactly one `.cpp` file of a program. The replacements must be defined once, and they must
 *   not be `inline` - a second translation unit that includes the header is a 'multiple definition' linker error.
 * - The counters are plain numbers, not safe for several threads.
 * - The compiler may remove a `new` and its `delete` if it can see that nobody needs the memory - the standard allows
 *   it. Then there is nothing to count. As Debug that does not happen, so count as Debug.
 * - Do not combine it with AddressSanitizer: ASan brings its own `operator new` and `operator delete`.
 */

namespace cbl::heap {

    inline std::size_t allocations{0};      // calls of operator new, all forms
    inline std::size_t releases{0};         // calls of operator delete with a pointer that is not null
    inline std::size_t bytes{0};            // bytes asked for by these allocations
    inline int loggers{0};                  // number of living heap_log objects
    inline bool inside{false};              // true while a log line is printed - printing may allocate, too

    inline void* allocate(const std::size_t size, const char* what) {
        void* p{std::malloc(size == 0 ? 1 : size)};     // `new` must return a unique address, even for 0 bytes
        if (p == nullptr) {
            throw std::bad_alloc{};
        }
        ++allocations;
        bytes += size;
        if (loggers > 0 && !inside) {
            inside = true;
            std::cout << " +|   " << what << ' ' << size << " bytes at " << p << '\n';
            inside = false;
        }
        return p;
    }

    inline void release(void* p, const char* what) noexcept {
        if (p == nullptr) {
            return;
        }
        ++releases;
        if (loggers > 0 && !inside) {
            inside = true;
            std::cout << " -|   " << what << " at " << p << '\n';
            inside = false;
        }
        std::free(p);
    }

}

// The replacements - not in a namespace and not `inline`, that is what the standard asks for. The ones with a size are
// what the compiler calls when it knows the size of the block it gives back (sized deallocation).
void* operator new(const std::size_t size) { return cbl::heap::allocate(size, "new"); }
void* operator new[](const std::size_t size) { return cbl::heap::allocate(size, "new[]"); }
void operator delete(void* p) noexcept { cbl::heap::release(p, "delete"); }
void operator delete[](void* p) noexcept { cbl::heap::release(p, "delete[]"); }
void operator delete(void* p, std::size_t) noexcept { cbl::heap::release(p, "delete"); }
void operator delete[](void* p, std::size_t) noexcept { cbl::heap::release(p, "delete[]"); }

/*
 * The counters as a watch: what happened since it was created, or since the last `reset()`.
 */
class heap_watch {
public:
    void reset() {
        allocations_ = cbl::heap::allocations;
        releases_ = cbl::heap::releases;
        bytes_ = cbl::heap::bytes;
    }

    [[nodiscard]] std::size_t allocations() const { return cbl::heap::allocations - allocations_; }
    [[nodiscard]] std::size_t releases() const { return cbl::heap::releases - releases_; }
    [[nodiscard]] std::size_t bytes() const { return cbl::heap::bytes - bytes_; }

    // Blocks allocated and not yet released - since the watch was created. Not 0 at the end: a leak, or a block that
    // was already there before.
    [[nodiscard]] long long live() const {
        return static_cast<long long>(allocations()) - static_cast<long long>(releases());
    }

private:
    std::size_t allocations_{cbl::heap::allocations};
    std::size_t releases_{cbl::heap::releases};
    std::size_t bytes_{cbl::heap::bytes};
};

/*
 * Prints every allocation and release while it lives - RAII, see unit 0x03.
 */
class heap_log {
public:
    heap_log() { ++cbl::heap::loggers; }
    ~heap_log() { --cbl::heap::loggers; }

    heap_log(const heap_log&) = delete;
    heap_log& operator=(const heap_log&) = delete;
};

#endif // CBL_HEAP_WATCH_HPP
