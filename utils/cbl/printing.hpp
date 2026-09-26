// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

#ifndef CBL_PRINTING_HPP
#define CBL_PRINTING_HPP

#include <iostream>                         // cout and endl
#include <source_location>                  // source_location (C++20)
#include <string>                           // string
#include <string_view>                      // string_view

/*
 * Reduces a full function signature to the qualified function name, i.e. everything
 * left of the parameter list, without the return type:
 *
 *      "void foo::S::bar(int)"     ->  "foo::S::bar"
 *      "int main()"                ->  "main"
 *
 * The spelling of a signature is implementation-defined, and the compilers differ in
 * exactly the place that matters here — a function in an unnamed namespace is
 *
 *      gcc:    "void {anonymous}::define_and_init()"
 *      clang:  "void (anonymous namespace)::define_and_init()"
 *
 * so a scan for the FIRST '(' finds the namespace, not the parameter list. Both steps
 * below therefore work on bracket nesting rather than on the first delimiter they see.
 *
 * Note how nothing is copied: a string_view is a (pointer, size) pair onto the
 * characters owned by someone else — here the static string literal the compiler
 * generated for the signature. Every step only moves the view's boundaries inwards.
 *
 * 'constexpr' means this can also run at compile time; see the static_asserts below.
 */
constexpr std::string_view extract_function_name(const std::string_view signature) {
    /*
     * Step 1: cut at the '(' that OPENS THE PARAMETER LIST - found by matching
     * backwards from the last ')' in the signature, so neither "(anonymous namespace)"
     * to its left nor a trailing "const" or "[T = int]" to its right can mislead it.
     */
    std::size_t paren{std::string_view::npos};
    if (const std::size_t close{signature.rfind(')')}; close != std::string_view::npos) {
        int nesting{0};
        for (std::size_t i{close + 1}; i-- > 0; ) {
            const char c{signature[i]};
            if (c == ')') { ++nesting; }
            else if (c == '(' && --nesting == 0) { paren = i; break; }
        }
    }
    const std::string_view head{signature.substr(0, paren)};    // npos == "up to the end"

    /*
     * Step 2: drop the return type. It is separated from the name by the last space
     * OUTSIDE any bracket - 'std::pair<int, int> f' must not split at the space after
     * the comma, and "(anonymous namespace)" must not split at its own space either.
     * Constructors have no return type and no space, in which case 'start' stays 0.
     */
    int depth{0};
    std::size_t start{0};
    for (std::size_t i{0}; i < head.size(); ++i) {
        const char c{head[i]};
        if (c == '<' || c == '(' || c == '[') { ++depth; }
        else if (c == '>' || c == ')' || c == ']') { --depth; }
        else if (c == ' ' && depth == 0) { start = i + 1; }
    }
    std::string_view name{head.substr(start)};

    /*
     * Step 3: cosmetics. Clang writes the reference of a return type on the name
     * ("std::ostream &operator<<"), and an unnamed namespace is noise in a header
     * line - dropping it also keeps the printed name identical to the identifier in
     * the source, which is what the Codebook plugin searches for when it places its
     * output-linking gutter icons.
     */
    while (!name.empty() && (name.front() == '&' || name.front() == '*')) {
        name.remove_prefix(1);
    }
    for (const std::string_view tag : {"(anonymous namespace)::", "{anonymous}::"}) {
        if (name.starts_with(tag)) { name.remove_prefix(tag.size()); break; }
    }
    return name;
}

static_assert(extract_function_name("void foo::S::bar(int)") == "foo::S::bar");
static_assert(extract_function_name("int main()") == "main");
static_assert(extract_function_name("void f<std::pair<int, int>>(int)") == "f<std::pair<int, int>>");
static_assert(extract_function_name("S::S(int)") == "S::S");
// unnamed namespace, in both spellings - this is what silently printed nothing
static_assert(extract_function_name("void (anonymous namespace)::define_and_init()") == "define_and_init");
static_assert(extract_function_name("void {anonymous}::define_and_init()") == "define_and_init");
// trailing qualifiers and template arguments sit behind the parameter list
static_assert(extract_function_name("double Temperature::as_fahrenheit() const") == "Temperature::as_fahrenheit");
static_assert(extract_function_name("void join(const T &) [T = std::vector<int>]") == "join");
static_assert(extract_function_name("void f(T) [with T = int]") == "f");
// operators: '(' and '<' in the NAME must not be read as brackets of their own
static_assert(extract_function_name("std::ostream &operator<<(std::ostream &, const T &)") == "operator<<");
static_assert(extract_function_name("double polynomial::operator()(double) const") == "polynomial::operator()");

/*
 * Prints the name of the calling function and a separator line of matching length,
 * e.g. called at the beginning of a function.
 *
 * The trick is the default argument: 'std::source_location::current()' is evaluated
 * at the *call site*, not here, so each caller silently passes its own location.
 * That is exactly what a macro using '__func__' would have done — but as a real
 * function: it can be namespaced, overloaded, stepped through in a debugger, and
 * it does not pollute the preprocessor namespace.
 *
 * Note that 'function_name()' yields the full signature, e.g. "void foo::S::bar(int)",
 * in an implementation-defined spelling, hence the trimming above.
 *
 * 'inline' allows this definition to appear in every translation unit including this header.
 */
inline void print_function_header(
        const std::source_location location = std::source_location::current()) {
    const std::string_view name{extract_function_name(location.function_name())};
    // 'endl' on purpose: flush the heading now, so it is still visible if the
    // snippet crashes later (and unflushed output is lost with the process).
    std::cout << "\n" << name << "\n" << std::string(name.size(), '=') << std::endl;
}

/*
 * 'location' also carries file_name(), line() and column() — handy for logging:
 *
 *   std::cout << location.file_name() << ":" << location.line() << std::endl;
 */

#endif // CBL_PRINTING_HPP
