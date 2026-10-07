// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

// One solution for task 'Dover Town', see ../tasks.md.

#include <iostream>
#include <cstdint>
#include <utility>
#include <cstdlib>

using std::cout, std::ostream, std::uint8_t, std::uint16_t, std::to_underlying;

enum class light : uint8_t { red, red_yellow, green, yellow };

// Prefix: the next phase, after `yellow` comes `red`. A value without a name - see the extension - also goes to `red`.
light& operator++(light& l) {
    switch (l) {
        case light::red:        l = light::red_yellow; return l;
        case light::red_yellow: l = light::green;      return l;
        case light::green:      l = light::yellow;     return l;
        case light::yellow:     l = light::red;        return l;
    }
    l = light::red;
    return l;
}

// Extension: postfix - returns the old phase, `l` has the new one.
light operator++(light& l, int) {
    const light old{l};
    ++l;
    return old;
}

// No `default:` - `-Wswitch` warns if a phase is added and forgotten here. A value without a name gets a line of its
// own after the `switch`: the extension.
ostream& operator<<(ostream& os, const light l) {
    switch (l) {
        case light::red:        return os << "red";
        case light::red_yellow: return os << "red-yellow";
        case light::green:      return os << "green";
        case light::yellow:     return os << "yellow";
    }
    return os << "unknown (" << static_cast<int>(to_underlying(l)) << ')';
}

// As Release, gcc and clang turn the `switch` into a table: a check `cmp dil, 3` / `ja`, then a load from an array
// with the four numbers - see the session.
int seconds_of(const light l) {
    switch (l) {
        case light::red:        return 30;
        case light::red_yellow: return 3;
        case light::green:      return 25;
        case light::yellow:     return 4;
    }
    return 0;
}

struct crossing {
    light north_south;
    light east_west;
    uint16_t remaining;
};

// Without `: uint8_t`, a `light` is an `int`: 4 + 4 + 2 bytes, and 2 bytes of padding to a multiple of 4.
enum class wide_light { red, red_yellow, green, yellow };

struct wide_crossing {
    wide_light north_south;
    wide_light east_west;
    uint16_t remaining;
};

int main() {
    light l{light::red};
    int total{0};
    for (int phase{0}; phase < 4; ++phase) {
        cout << l << ": " << seconds_of(l) << " s\n";
        total += seconds_of(l);
        ++l;
    }
    cout << "one cycle: " << total << " s, back at " << l << '\n';

    cout << "sizeof(crossing) = " << sizeof(crossing) << ", sizeof(wide_crossing) = " << sizeof(wide_crossing) << '\n';

    // Extension: postfix `++`.
    const light before{l++};
    cout << "l++ returned " << before << ", l is " << l << '\n';

    // Extension: a value without a name. Without the lines after the `switch`, both functions would run off their end
    // for 9 - undefined behavior for a function that returns a value. gcc warns (`-Wreturn-type`), clang does not: it
    // trusts a `switch` that has a `case` for every name.
    const light odd{static_cast<light>(9)};
    cout << "odd: " << odd << ", " << seconds_of(odd) << " s\n";

    return EXIT_SUCCESS;
}
