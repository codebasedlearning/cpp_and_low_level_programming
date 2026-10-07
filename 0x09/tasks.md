[© A.Voß, FH Aachen, codebasedlearning.dev](mailto:info@codebasedlearning.dev)

# Unit 0x09 - Tasks

## How to work on the tasks

- Work in a project of your own, outside this repository, as before - one `add_executable` per task.
- Some tasks look at what the compiler makes of a lambda, or at the machine code: C++ Insights (cppinsights.io),
  Compiler Explorer (godbolt.org) and `nm`.
- Measure as Release, count allocations as Debug - copy `stopwatch.hpp` and `heap_watch.hpp` from `utils/cbl` into your
  project where a task needs them.
- When you are done, compare your solution with the one in `iv_solutions` (once available), and ask an LLM of your
  choice (ChatGPT, Claude, ...) to review it - paste the task and your code. What does it criticize, and is it right?
  Check its claims before you believe them. If asked, post on Slido what you have learned.

<hr>

### 👉 Task 'Roarport'

Three lambdas. Write a lambda for each of the following:

- It adds three `double`s and returns the sum.
- It tests whether an `int` lies in the interval `[a, b]`, where `a` and `b` are local variables, captured by copy.
- It takes no argument, and sets a local `int` variable `z` to `-z` (an academic example).
- Call each of them, and print the results - for the third one, print `z` before and after.
- Predict `sizeof` of each lambda, then check.

Extension:

- Capture `z` by copy instead. What does the compiler say? Make it compile with `mutable` - and what is `z` after the
  call now?
- Write the class of the second lambda by hand: two data members and an `operator()`. Paste your lambda into C++
  Insights and compare.
- Store the second lambda in a `std::function<bool(int)>`. What is `sizeof` of the `std::function` - and does it
  allocate? Count with `heap_watch`, as Debug.

<hr>

### 👉 Task 'Kreley'

Function pointers and `std::function`:

- Define a type `quotient_function` for a pointer to a function that takes two `int`s, `x` and `y`, and returns a
  `double` - with `using`.
- Write a function `q` that returns `x / y` - as a `double`, without an integer division.
- Initialize a variable of type `quotient_function` with `q`, and call `q` through it.
- Do the same with a `std::function`.
- Print `sizeof` of both, and the address of `q`, of a global variable, of a local variable, and of a block on the heap.
  Which two are close to each other?

Extension:

- A calculator: a `std::array` of four function pointers for `+`, `-`, `*` and `/` on `double`s. Read a line like
  `3 * 4` from the console, pick the function by the position of the operator in `"+-*/"`, and call it.
- Paste the part that picks and calls into Compiler Explorer. Find the indirect call. What happens with `-O2` when the
  operator is a constant?

<hr>

### 👉 Task 'Yrouwood'

A fixed point, iterated:

- Write a function `approx` with three parameters: a start value `x0`, a function `f` from `double` to `double`, and an
  `eps`. It computes `x = f(x)`, starting with `x = x0`, as long as the new value differs from the one before by more
  than `eps`, and returns the last value.
- Test it with Heron's method for the square root of `a` (see Wikipedia): `f(x) = (x + a / x) / 2`, as a lambda that
  captures `a`. Take `a = 2` and `a = 4`, `x0 = 1` and `eps = 1e-10`.
- Write `approx` three times: `f` as a template parameter, as a function pointer `double (*)(double)`, and as a
  `const std::function<double(double)>&`. Which of them take your Heron lambda - and why not all of them?
- Make the function pointer version compute square roots anyway. What did you have to give up?

Extension:

- Compute the square roots of 1 to 1,000,000 with each version, and measure each as Release with `stopwatch`. Predict
  the order first. Explain what you see.

<hr>

### 👉 Task 'Pleim'

Binomial coefficients, without a loop:

- Fill a `vector` with the binomial coefficients "n choose k" for a fixed `n`, `k` from 0 to `n` - for `n = 4`: 1, 4, 6,
  4, 1.
- Use only algorithms - `std::generate`, `std::transform`, `std::iota`, `std::for_each`, the `std::ranges` versions -
  and lambdas: no `for`, no `while`, no range-based `for`. Print it the same way.

Extension:

- Pascal's triangle up to `n = 10`, every row computed from the one before: the inner numbers are the sums of
  neighbors - `std::transform` with two ranges, the row and the row shifted by one, and `std::plus<>`. One loop, for the
  rows, is allowed.

<hr>

### 👉 Task 'Silver Lake'

`qsort` against `std::sort`. Fill a `vector` with a million random `int`s (`std::mt19937` and a
`std::uniform_int_distribution`), and sort a fresh copy of it five times:

- `std::qsort`, with a comparison function on two `const void*`.
- `std::sort`, with a pointer to a function `bool less_than(int a, int b)`.
- `std::sort`, with a lambda.
- `std::sort`, with the same lambda in a `std::function<bool(int, int)>`.
- `std::sort` without a comparison.
- Predict the order of the five, then measure each with `stopwatch`, as Release. Explain what you see.
- How many comparisons does `std::sort` make for a million numbers? Count them with a lambda that captures a counter by
  reference. And `qsort`?

Extension:

- Build as Debug and run `nm -C` on your object file (unit 0x04). How many versions of the sort function of your library
  do you find, and for which comparisons?

<hr>

### 👉 Task 'Hollow Creek'

Dangling lambdas. A factory for counters:

```cpp
auto make_counter() {
    int count{0};
    return [&count] { return ++count; };
}
```

- Predict what `c()` returns when you call it three times, for `const auto c = make_counter();`. Then run it - as Debug
  and as Release, with gcc and with clang if you can. Read the warnings.
- Fix `make_counter` - with an init-capture and `mutable`. Put five counters into a `std::vector<std::function<int()>>`,
  and show that each of them counts on its own.

Now a class whose objects hand out callbacks:

```cpp
class sensor {
public:
    explicit sensor(const double value) : value_{value} {}
    std::function<double()> reader() const { return [this] { return value_; }; }
private:
    double value_;
};
```

- Put two sensors into a `std::vector<sensor>`, and the readers of both into a `std::vector<std::function<double()>>`.
  Call the readers.
- Add a third sensor with `emplace_back`, and call the first two readers again. Predict first. Then print the addresses
  the readers refer to and the addresses of the sensors. If your toolchain has AddressSanitizer (unit 0x05), build with
  it and read the report.
- Fix it in three ways: with `reserve`, with a `std::vector<std::unique_ptr<sensor>>`, and with a reader that does not
  need `this`. What does each fix cost, and what does it promise?

Extension:

- Delete the copy and move constructors and assignments of `sensor`. Does `std::vector<sensor>` with `emplace_back`
  still compile? And `std::deque<sensor>` - why is that different?

<hr>

### 👉 Task 'AI' - Two Opinions

Two answers to the question "What is a lambda in C++, and what does it cost?", as an LLM might write them:

> **Answer A:** A lambda is an object of a class that the compiler generates for it: the captured variables become its
> data members, and its body becomes the `operator()` of the class. Because every lambda has its own type, an algorithm
> like `std::sort` is instantiated for it, and the call can be inlined - that is why a lambda is usually faster than a
> function pointer. A lambda without captures takes no memory at all - `sizeof` is 0 - and it can be converted to a
> plain function pointer.

> **Answer B:** A lambda that captures by reference stores the addresses of the variables, so it must not outlive them.
> `std::function` can hold any lambda without an allocation, because it stores the lambda inside its own object. A call
> through a `std::function` is an indirect call, which the compiler usually cannot inline. So in a hot loop, pass a
> lambda as a template parameter rather than as a `std::function`.

- Go through both answers statement by statement: which ones are right, which are wrong?
- Check them: `sizeof`, `heap_watch`, and the machine code of a call through a `std::function`.
- If you have an LLM at hand, ask it the same question. Does it make one of these mistakes?

Our results: [answers](../docs/answers.md#ai-0x09).

<hr>

### 👉 Task 'Couch Potato' - Recurring homework

- If you have not completed the tasks in the exercise, complete them at home.
- Work through the required study: all `study_*` files in `iii_follow_up`.
- If you are curious: the `tinker_*` files there.

<hr>

## Comprehension Check

Answers: [comprehension check 0x09](../docs/answers.md#check-0x09).

- I can say where the code of a function is, what a function pointer holds, and what a call through it looks like in the
  machine.
- I can write the type of a function pointer - as a variable, a parameter and a return type, with and without `using` -
  and use a table of them.
- I can explain how `qsort` gets its comparison, and what that costs.
- I can write the class behind a lambda by hand, and read what C++ Insights shows for it.
- I can predict `sizeof` of a lambda from its captures.
- I can explain where a captured copy lives, and what a captured reference holds.
- I can explain why a `mutable` lambda keeps its state, and what a copy of it does.
- I know that every lambda has its own type - and why that makes `std::sort` with a lambda fast.
- I can explain which lambdas convert to a function pointer, and why the others cannot.
- I can compare a template parameter, a function pointer and a `std::function` as a parameter: machine code, size,
  allocations, speed.
- I can explain what a `std::function` stores, when it allocates, and what happens when it is empty.
- I can explain why a lambda that captured a reference or `this` can dangle - and three ways to prevent it.
- I can move a `unique_ptr` into a lambda, and say what that does to the lambda.
- I can use the everyday algorithms with lambdas, and I know the traps of `accumulate` and `remove_if`.
- I can write generic lambdas, init-captures, an immediately invoked lambda and a recursive lambda.
- I know what `[=]` captures in a member function, and the difference between `[this]` and `[*this]`.
- I can write a list of callbacks with `std::function` - and say why a widget that registers `[this]` must not be copied
  or moved.
