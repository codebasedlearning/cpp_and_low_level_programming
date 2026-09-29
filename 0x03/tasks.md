[© A.Voß, FH Aachen, codebasedlearning.dev](mailto:info@codebasedlearning.dev)

# Unit 0x03 - Tasks

## How to work on the tasks

- Work in a project of your own, outside this repository, as before - one `add_executable` per task.
- Write each task in one file first, class and test code together. Some extensions ask you to split the class into a
  header and a source file afterwards, as in the session: then the task gets a folder of its own, and all its `.cpp`
  files go into its `add_executable`.
- When you are done, compare your solution with the one in `iv_solutions` (once available), and ask an LLM of your
  choice (ChatGPT, Claude, ...) to review it - paste the task and your code. What does it criticize, and is it right?
  Check its claims before you believe them. If asked, post on Slido what you have learned.

<hr>

### 👉 Task 'Ravencastle'

- Write a class `fraction` with a numerator and a denominator, both `int`.
- A constructor takes both values and initializes the members in the member initializer list. A denominator of 0
  throws a `std::invalid_argument` - such a fraction is never born.
- Add the getters `num()` and `denom()`, and `value()`, which returns the fraction as a `double`. Which of them are
  `const`?
- Write meaningful test code, including a fraction with denominator 0.

Extension:

- Reduce the fraction in the constructor, with `std::gcd` from `<numeric>`: `fraction{2, 4}` becomes 1/2.
- Predict `sizeof(fraction)`, then check it.
- Split it: the class into `fraction.hpp`, its member functions into `fraction.cpp`, the test code into `main.cpp`.

<hr>

### 👉 Task 'Stone Ridge'

- Write a class `point` with two private `double` coordinates, a constructor, and a member function `where()` that
  prints `this`.
- Predict `sizeof(point)`, then check it.
- Create a `point p` and print `&p` and `p.where()`. Then create a `std::vector<point>` with three points and call
  `where()` for each element. How far apart are the addresses, and why?

Extension:

- Write a free function `scale(point_data* p, double f)` for a `struct point_data` with the same two members, and a
  member function `point::scale(double f)` defined outside the class. Compare both in Compiler Explorer (godbolt.org).

<hr>

### 👉 Task 'Lucky Rock'

- Write a class `tracer` with a `string` name that prints one line in each special member function: `ctor a`,
  `copy a'` (a copy gets a `'` added to the name), `assign a' from c`, `dtor a`.
- Predict, line by line, what this `main` prints - on paper, before you run it:

```cpp
tracer make(const string& name) {
    return tracer{name};
}

void show(const tracer t) {
    cout << "show " << t.name() << '\n';
}

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
```

- Run it and compare. Where were you wrong, and why?

Extension:

- Change `show` to take a `const tracer&`. Which lines disappear?

<hr>

### 👉 Task 'Meadow River'

- Turn the struct `polynom` from 'Elkford' (unit 0x02) into a class `polynom`. The coefficients are private.
- Turn the free function `eval` into a member function. Is it `const`?
- Add a member function `at(i)` that returns the i-th coefficient, and throws a `std::out_of_range` if `i` is invalid.
- Reuse your 'Elkford' tests.

Extension:

- Write a free function `add` that adds two polynomials and returns the result - how does it get to the coefficients,
  which are private now?
- Test with dimension 4.
- Split it into `polynom.hpp`, `polynom.cpp` and `main.cpp`. Where does the `constexpr` dimension go?

<hr>

### 👉 Task 'Coral Creek'

Sometimes a value is optional: a temperature sensor, for example, may not have produced data yet. Instead of a magic
number like -1 for "no value", wrap the `int` in a class `optional_int` that knows whether it is set - similar to
`std::optional<int>`.

- A default constructor creates an empty `optional_int`; a constructor with an `int` creates a set one.
- `has_value()` says whether a value is set.
- `value()` returns the value if it is set, otherwise it throws a `std::runtime_error`.
- `value_or(fallback)` returns the value if it is set, otherwise `fallback`.
- `set(v)` sets a value, `clear()` removes it.
- Write test code.

Extension:

- Predict `sizeof(optional_int)`, then check it - and compare it with `sizeof(int)` and `sizeof(std::optional<int>)`
  (from `<optional>`). Where are the extra bytes?
- Split it into `optional_int.hpp`, `optional_int.cpp` and `main.cpp`.

<hr>

### 👉 Task 'Quarry Bend'

Take your 'Ravencastle' project, split into a header and a source file as in its extension, and break it on purpose,
one change at a time. Read each message: which function does
it name, and in which file is it missing - or found twice? Undo each change before the next one.

- Remove `fraction.cpp` from the `add_executable`.
- Declare a member function `double inverse() const;` in the header, call it in `main.cpp`, but do not define it.
- Add a free function with a body to the header, below the class, e.g. `int twice(const int n) { return 2 * n; }`.
  Both `fraction.cpp` and `main.cpp` include the header.

Extension:

- Remove the `#pragma once` from the header, and include it twice in `main.cpp`. Is this a linker error, too?

Our messages: [answers](../docs/answers.md#check-0x03), the point on linker errors.

<hr>

### 👉 Task 'Ashford Mill'

- Write a class `scope_timer`: it takes a label in the constructor, starts a `stopwatch` (copy `utils/cbl/stopwatch.hpp`
  into your project), and prints the label and the elapsed time in its destructor.
- A `scope_timer` should not be copied - make sure the compiler says so.
- Use it in a function that does some measurable work and then, depending on a parameter, throws. Call it both ways.
  Does the timer print in both cases?

Extension:

- Create two timers in one block, and a third one in an inner block. Predict the order of the lines, then check it.

<hr>

### 👉 Task 'AI' - Two Opinions

Two answers to the question "Does a C++ object carry hidden overhead - and how big is
`class point { double x_; double y_; public: double length() const; ~point(); };`?", as an LLM might write them:

> **Answer A:** Member functions are stored once, in the code, not in every object - so only the data members count.
> `point` has 16 bytes: two `double`s. But because it has a user-defined destructor, the compiler adds a hidden pointer
> to it, so that the right destructor can be found at runtime. That makes 24 bytes.

> **Answer B:** 16 bytes. Neither member functions nor constructors nor destructors take space in the object, and
> neither does `private`. Only data members count - so a class without data members takes no memory at all: its
> `sizeof` is 0.

- Go through both answers statement by statement: which ones are right, which are wrong?
- Check them with a small program of your own: `sizeof` of `point` with and without the destructor, and of an empty
  class.
- If you have an LLM at hand, ask it the same question. Does it make one of these mistakes? And when does an object
  get a hidden pointer after all?

Our results: [answers](../docs/answers.md#ai-0x03).

<hr>

### 👉 Task 'AI' - Explain the Machine Code

This time the LLM does not answer a question - it explains something you give it, and you check the explanation against
the machine.

```cpp
struct tracer {
    explicit tracer(int id);
    ~tracer();
};

void work();

void f() {
    tracer a{1};
    tracer b{2};
    work();
}
```

- `tracer` and `work` are only declared, so the compiler cannot inline them - every call stays visible. Before you look:
  which calls do you expect in `f`, and in which order?
- Paste the code into Compiler Explorer (godbolt.org), x86-64 gcc, `-O1`, and count the calls of the destructor. More
  than two?
- Give the code and the assembly to an LLM of your choice: "Explain this machine code. Why are there more destructor
  calls than objects?"
- Check its explanation instead of believing it. Pick two of its claims and test them - e.g. give `tracer` and `work`
  bodies that print, let `work` throw, and set a breakpoint in the destructor: which of the calls are you in? What
  happens if the constructor of `b` throws?
- Where was the explanation right, where vague, where wrong?

Our results: [answers](../docs/answers.md#ai2-0x03).

<hr>

### 👉 Task 'Couch Potato' - Recurring homework

- If you have not completed the tasks in the exercise, complete them at home.
- Work through the required study: all `study_*` files in `iii_follow_up`.
- If you are curious: the `tinker_*` files there.

<hr>

## Comprehension Check

Answers: [comprehension check 0x03](../docs/answers.md#check-0x03).

- I can explain what an object is in memory, and what `sizeof` of a class includes - and what not.
- I can explain what `this` is, how a member function knows its object, and what a `const` member function promises.
- I can write constructors with a member initializer list, and I know in which order the members are initialized.
- I know when a constructor should be `explicit`, and what an implicit conversion creates.
- I can tell a copy construction from a copy assignment, and I know what the generated versions do.
- I can say exactly when a destructor runs: at the `}`, at the `;`, and during stack unwinding.
- I know what happens when a constructor throws, and why a destructor must not throw.
- I can explain RAII, and why C++ needs no `finally`.
- I can split a class into a header and a source file, and I can read the typical linker errors.
- I can write getters that neither copy needlessly nor give away the members.
- I know which special member functions the compiler generates, and how to `= default` or `= delete` them.
