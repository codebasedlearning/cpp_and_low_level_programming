// (C) A.Voß, a.voss@fh-aachen.de, info@codebasedlearning.dev

/* ---- Preamble ----
 *
 * Teaching Focus
 * - The algorithms of the standard library take the "what" as a lambda and do the "how": search, count, test,
 *   transform, add up, sort, remove, find the smallest - the everyday loops, each with a name.
 * - The classic form takes two iterators, `std::ranges` takes the container - and a projection, a key to sort or
 *   search by.
 * - Two traps: the start value of `accumulate` decides the type of the sum, and `remove_if` removes nothing - it
 *   moves the elements to keep to the front.
 * - With a lambda, an algorithm is instantiated for it and inlined: the same machine code as the loop by hand.
 */

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <numeric>                          // for accumulate, iota
#include <iterator>                         // for back_inserter
#include <cstdlib>
#include <cbl/printing.hpp>

using std::cout, std::string, std::vector;


/* ---- Content ---- */

namespace {

    /* --- `print_all` --- Prints the elements of a vector in one line. */
    template <typename T>
    void print_all(const vector<T>& values) {
        for (const T& v : values) {
            cout << ' ' << v;
        }
        cout << '\n';
    }

    /* --- `search_and_count` ---
     * `find_if` returns an iterator to the first element for which the lambda says `true` - or `end()` (see previous
     * snippets: "not found"). `count_if` counts them. `all_of`, `any_of` and `none_of` answer with a `bool`, and stop
     * as soon as the answer is known.
     * - !![#algorithms]
     */
    void search_and_count() {
        print_function_header();

        const vector<int> numbers{5, 7, 4, 2, 8, 6, 1, 9, 0, 3};
        const auto first_even{std::find_if(numbers.begin(), numbers.end(), [](const int n) { return n % 2 == 0; })};
        cout << " 1| first even: " << *first_even << " at index " << first_even - numbers.begin() << '\n';
        const auto greater_than_six{std::count_if(numbers.begin(), numbers.end(), [](const int n) { return n > 6; })};
        cout << " 2| greater than 6: " << greater_than_six << '\n';
        cout << " 3| all >= 0: " << std::ranges::all_of(numbers, [](const int n) { return n >= 0; }) << ", any > 8: "
             << std::ranges::any_of(numbers, [](const int n) { return n > 8; }) << ", none < 0: "
             << std::ranges::none_of(numbers, [](const int n) { return n < 0; }) << '\n';
    }

    /* --- `transform_and_add_up` ---
     * `transform` calls the lambda for every element and writes the results - here to the end of another vector,
     * through `back_inserter`, which calls `push_back` for every result. `accumulate` adds up, starting with the value
     * you give it - and that value decides the type of the sum: `0` makes it an `int`, and every `double` is cut on
     * its way in. `0.0` is what you meant. A lambda as fourth argument replaces the `+`.
     */
    void transform_and_add_up() {
        print_function_header();

        const vector<double> prices{1.5, 2.25, 0.75};
        vector<double> with_tax;
        std::transform(prices.begin(), prices.end(), std::back_inserter(with_tax), [](const double p) {
            return p * 1.19;
        });
        cout << " 1| with tax:";
        print_all(with_tax);

        cout << " 2| sum with 0: " << std::accumulate(prices.begin(), prices.end(), 0) << ", with 0.0: "
             << std::accumulate(prices.begin(), prices.end(), 0.0) << '\n';
        cout << " 3| product: "
             << std::accumulate(prices.begin(), prices.end(), 1.0, [](const double a, const double b) { return a * b; })
             << '\n';
    }

    /* --- `person` --- A name and an age. */
    struct person {
        string name;
        int age;
    };

    /* --- `print_people` --- Prints name and age of everybody. */
    void print_people(const vector<person>& people) {
        for (const person& p : people) {
            cout << ' ' << p.name << '(' << p.age << ')';
        }
        cout << '\n';
    }

    /* --- `sort_by_a_key` ---
     * A lambda that compares by the age sorts by the age. `stable_sort` keeps people of the same age in the order they
     * had before; `sort` may shuffle them. `ranges::sort` takes a projection: `&person::name` - "sort by this member" -
     * and compares the names with the default `<`; `std::ranges::greater{}` instead of the default turns it round.
     */
    void sort_by_a_key() {
        print_function_header();

        vector<person> people{{"Kim", 34}, {"Alex", 27}, {"Sam", 34}, {"Robin", 19}, {"Jo", 27}};
        std::stable_sort(people.begin(), people.end(), [](const person& a, const person& b) { return a.age < b.age; });
        cout << " 1| by age:";
        print_people(people);

        std::ranges::sort(people, {}, &person::name);
        cout << " 2| by name:";
        print_people(people);

        std::ranges::sort(people, std::ranges::greater{}, &person::age);
        cout << " 3| oldest first:";
        print_people(people);
    }

    /* --- `remove_elements` ---
     * `remove_if` cannot remove anything - it has two iterators, not the vector. It moves the elements to keep to the
     * front, in their order, and returns where the rest begins; the size is unchanged, and what is behind is left
     * over. `erase` cuts it off. Together: the erase-remove idiom. C++20 has both in one: `std::erase_if`.
     */
    void remove_elements() {
        print_function_header();

        vector<int> numbers{5, 7, 4, 2, 8, 6, 1, 9, 0, 3};
        const auto rest{std::remove_if(numbers.begin(), numbers.end(), [](const int n) { return n % 2 == 0; })};
        cout << " 1| after remove_if, size " << numbers.size() << ":";
        print_all(numbers);
        numbers.erase(rest, numbers.end());
        cout << " 2| after erase, size " << numbers.size() << ":";
        print_all(numbers);

        vector<int> more{5, 7, 4, 2, 8, 6, 1, 9, 0, 3};
        std::erase_if(more, [](const int n) { return n > 4; });
        cout << " 3| erase_if:";
        print_all(more);
    }

    /* --- `find_extremes` ---
     * `min_element` and `max_element` return an iterator, not the value - there may be no element at all, and then
     * it is `end()`. With a lambda, "smallest" means what the lambda says: here the shortest word.
     */
    void find_extremes() {
        print_function_header();

        const vector<string> words{"lambda", "is", "a", "struct", "with", "operator"};
        const auto longest{std::ranges::max_element(words, [](const string& a, const string& b) {
            return a.size() < b.size();
        })};
        const auto shortest{std::ranges::min_element(words, {}, &string::size)};
        cout << " 1| longest: " << *longest << ", shortest: " << *shortest << '\n';
    }

    /* --- `generate_values` ---
     * `iota` fills with counting values. `generate` calls the lambda for every element and stores what it returns - a
     * `mutable` lambda can keep state from call to call (see the session): here, the last two Fibonacci numbers.
     * `for_each` calls the lambda for every element, for what it does - the range-based `for` usually reads better.
     */
    void generate_values() {
        print_function_header();

        vector<int> counting(8);
        std::iota(counting.begin(), counting.end(), 1);
        cout << " 1| iota:";
        print_all(counting);

        vector<int> fibonacci(10);
        std::ranges::generate(fibonacci, [a = 0, b = 1]() mutable {
            const int next{a};
            a = b;
            b = next + b;
            return next;
        });
        cout << " 2| fibonacci:";
        print_all(fibonacci);

        int sum{0};
        std::ranges::for_each(fibonacci, [&sum](const int n) { sum += n; });
        cout << " 3| sum: " << sum << '\n';
    }

}

/* --- The loop by hand ---
 * An algorithm is a function template (see previous snippets). Called with a lambda, it is instantiated for the
 * lambda's class, and the lambda is inlined - the result is the loop you would have written. The algorithm is not
 * slower; it is harder to get wrong, and its name says what the loop does. Where no algorithm fits - two things at
 * once, an early `break` with a result - the loop by hand is fine. More at cppreference: "Algorithms library".
 */

/* --- `main` --- */
int main() {
    search_and_count();
    transform_and_add_up();
    sort_by_a_key();
    remove_elements();
    find_extremes();
    generate_values();

    return EXIT_SUCCESS;
}
