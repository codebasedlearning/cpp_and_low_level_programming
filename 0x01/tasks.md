[© A.Voß, FH Aachen, codebasedlearning.dev](mailto:info@codebasedlearning.dev)

# Unit 0x01 - Tasks

## How to work on the tasks

- Work in a project of your own, outside this repository - e.g. in CLion via
  'New Project', 'C++ Executable'. Create one file per task, named after the task, e.g.
  `westrosecliff.cpp`, and add a target for it: `add_executable(westrosecliff westrosecliff.cpp)`.
  Or build it with `g++` or a `makefile` of your own - see `i_preparation`.
- Reading input from the console comes in the next unit. Until then, define your input values
  as `const` variables at the top of `main` - change them and rebuild to test other cases.
- Each task lists the files it builds on under "Uses". Work through them first.
- After the deadline, compare your solutions with the ones in `iv_solutions`.

<hr>

### 👉 Task 'West Rose Cliff'

Uses: `d_var_init`, `c_control_flow`

- Define a `const int n` and a `const char c` with values of your choice.
- Test whether `n` is greater than `0` and whether `c` is an uppercase letter. Use only
  comparisons on the character itself, e.g. `'A' <= c`.
- Store both results in `bool` variables and output them.

Extension:

- Use the `?:` operator instead of an `if` where possible.
- `<cctype>` offers `std::isupper`. Look up why it takes an `int` and not a `char`.

<hr>

### 👉 Task 'Echo Valley'

Uses: `a_var_init_dive`

- Write a function `set_value` that defines an `int` with a value of your choice, and a
  function `read_value` that defines an `int` without a value and prints it.
- Call `set_value`, then `read_value`. Predict the output first.
- Build and run it once as Debug (`-O0`) and once as Release (`-O2`). Explain the difference
  in two sentences.

Extension:

- Add a second variable to `set_value`, before the first one. Does `read_value` still print
  your value? Why, or why not?

<hr>

### 👉 Task 'Little Harbor'

Uses: `b_var_init_string`

- Find the length from which on the characters of a `string` no longer lie inside the object.
- Start with an empty `string` and append one character at a time in a loop. Each time, print
  the length, the address of the object and the address of the characters (as in
  `b_var_init_string`).
- Compare your result with someone who uses another compiler or operating system.

Extension:

- Also print `s.capacity()`. What do you notice at the moment the characters move?

<hr>

### 👉 Task 'Blue Canyon'

Uses: `c_control_flow`, `d_functions`, `must_control_flow`

- Define `b` and `n` as `const` variables of an appropriate type. Calculate `b` to the power
  of `n` in a loop and output the result.
- Write a function `pot` that receives `b` and `n` and returns the result. Declare it before
  `main` and define it after `main`.
- Use `const` wherever possible.

Extension:

- Formulate the loop once as a `for` and once as a `while` loop.
- For `b=2`: from which `n` on is the result wrong? Why is there no error message? Compare
  with `unsigned int`.

<hr>

### 👉 Task 'Beverly Hollow'

Uses: `c_control_flow`, `d_functions`, optionally `may_int8`

- Define a `const` number `n` and calculate the first `n` Fibonacci numbers `f_i` iteratively,
  i.e. in a loop, not recursively, and output them. The rule is `f_i = f_(i-1) + f_(i-2)`
  with `f_0 = 0` and `f_1 = 1`.
- Find the first `i` for which `int` is no longer enough. Switch to `std::uint64_t`: how far do
  you get now?

Extension:

- `d_functions` computes `fib` recursively. Count how many calls `fib(30)` needs. Which version
  is faster, and why? Think of the stack frames.

<hr>

### 👉 Task 'Pine Point'

Uses: `d_functions`, `must_control_flow`, `must_assert`

- Write a function `is_prime` that receives a number and returns `true` or `false`.
- Test it for several numbers with `assert` from `<cassert>`, e.g. `assert(is_prime(7));`.
  A failing `assert` stops the program.
- Output all primes below a `const` limit, e.g. 100.

Extension:

- Count the primes below 1'000'000. Measure the runtime with `time ./pinepoint` in a terminal,
  once built with `-O0` and once with `-O2`. How big is the difference?

<hr>

### 👉 Task 'AI'

What would the AI say about it?

Pick one of your solutions and ask a GenAI tool of your choice for its opinion.

- What improvements are suggested?
- Do they make sense?

<hr>

### 👉 Task 'Couch Potato' - Recurring homework

- If you have not completed the tasks in the exercise, complete them at home.
- Work through the required study: `must_control_flow`, `must_string`, `must_assert`.
- If you are curious: `may_int8`, `may_goto`.

<hr>

## Comprehension Check

- I can create, build and run a C++ program, with CMake and with `make`.
- I can define and initialize variables, and I know why we prefer `{}`.
- I can explain what an uninitialized variable contains and why reading it is UB.
- I know that the sizes of types depend on the platform, and how to find them out.
- I know where the characters of a `string` live.
- I can declare and define functions, and I know what a call does on the stack.
- I can predict what happens on integer overflow.
- I know the control structures `if`, `for`, `while`, `do`-`while` and `switch`.
