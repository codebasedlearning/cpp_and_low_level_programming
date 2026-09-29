[© A.Voß, FH Aachen, codebasedlearning.dev](mailto:info@codebasedlearning.dev)

# Unit 0x01 — Tasks

## The Build Process

### Compiling, Linking and Running an Executable

Three steps turn a text file into a running program:

- **Compiling** — the compiler (`g++`, `clang++`, MSVC) translates one source file, after the preprocessor has pasted 
  in its `#include`s, into machine code for your processor. The result is an *object file* (`.o`, `.obj`): machine code,
  but not yet runnable — calls into other files and into the standard library, such as the output with `std::cout`,
  are still open ends.
- **Linking** — the linker joins the object files and the libraries into one *executable* and connects the open ends, 
  so that every call finds its function. Exactly one `main` must be among them.
- **Running** — the operating system loads the executable into memory and starts it; a bit of start-up code from the 
  C++ runtime then calls `main`. The value `main` returns goes back to the shell as the exit code.

`g++ a_helloworld.cpp -o a_helloworld.out` does compiling and linking in one go. The two steps separately:

```
g++ -c a_helloworld.cpp                     # compile only: a_helloworld.o
g++ a_helloworld.o -o a_helloworld.out      # link: a_helloworld.out
./a_helloworld.out                          # run
```

Two consequences. Errors come from different steps: a typo or a type error from the compiler, an `undefined reference`
(or `undefined symbols`) from the linker. And the executable is what runs, not the source: change the source and forget
to compile, and you run the old program.
More in the [glossary](../docs/glossary.md#compiler-and-linker); the linker gets its own look once programs consist
of several files.

### Make

Instead of compiling everything every time, use `make`. It checks what changed and only rebuilds what’s necessary.
The rules and dependencies live in a file called `makefile`. To use it, just type:

```
make
```

### CMake

Behind the scenes in many IDEs (like CLion), `CMake` orchestrates the build. CMake isn’t a build system itself — it
generates build files for one (e.g., makefiles). In practice, you mostly edit `../CMakeLists.txt` to declare sources,
targets and dependencies; CLion runs CMake and the build tools for you.

## Tasks

### How to work on the programming tasks

- Work in a project of your own, outside this repository. Create one file per task, named after the task, e.g.
  `westrosecliff.cpp`, and add a target for it in `CMakeLists.txt`.
  Or build it with `g++` or a `makefile` of your own — see `i_preparation`.
- Reading input from the console comes in the `follow_up` console-sample. Alternatively, define your input values
  as `const` variables at the top of `main` - change them and rebuild to test other cases.
- When you are done, compare your solution with the one in `iv_solutions` (once available), and ask an LLM of your
  choice (ChatGPT, Claude, ...) to review it - paste the task and your code. What does it criticize, and is it right?
  Check its claims before you believe them. If asked, post on Slido what you have learned.

<hr>

### 👉 Task 'Compile and Run'

Open a terminal, change into the `i_preparation` directory, and compile your first C++ file. Run

```
g++ a_helloworld.cpp -o a_helloworld.out
```

This produces an executable named `a_helloworld.out`. Now launch it:

```
./a_helloworld.out
```

If you skip the -o option, the compiler defaults to the very imaginative name `a.out` — which is fine until you have
three of them and can’t remember who’s who.

Remove `a_helloworld.out` and build with
```
make
```

<hr>

### 👉 Task 'Empty Canyon'

Create your first C++-project in your IDE. Create or copy a simple C++-source, compile and run it.

<hr>

### 👉 Task 'West Rose Cliff'

- Define a `const int n` and a `const char c` with values of your choice.
- Test whether `n` is greater than `0` and whether `c` is an uppercase letter. Use only comparisons on the character
  itself, e.g. `'A' <= c`.
- Store both results in `bool` variables and output them.

Extension:

- Use the `?:` operator instead of an `if` where possible.
- `<cctype>` offers `std::isupper`. Look up why it takes an `int` and not a `char`.

Review: paste the task and your solution into an LLM. What does it criticize, and is it right?

<hr>

### 👉 Task 'Echo Valley'

- Write a function `set_value` that (locally) defines an `int` with a value of your choice, and a function `read_value` 
  that (also locally) defines an `int` without a value and prints it.
- Call `set_value`, then `read_value`. Predict the output first.
- Build and run it once as Debug (`-O0`) and once as Release (`-O2`). Explain the difference in two sentences.

Extension:

- Add a second variable to `set_value`, before the first one. Does `read_value` still print your value? Why, or why not?

Review: paste the task and your solution into an LLM. What does it criticize, and is it right?

<hr>

### 👉 Task 'Little Harbor'

- Find the length from which on the characters of a `string` no longer lie inside the object.
- Start with an empty `string` and append one character at a time in a loop. Each time, print the length, the address of
  the object and the address of the characters (as in the session).
- Compare your result with someone who uses another compiler or operating system.

Extension:

- Also print `s.capacity()`. What do you notice at the moment the characters move?

Review: paste the task and your solution into an LLM. What does it criticize, and is it right?

<hr>

### 👉 Task 'Blue Canyon'

- Define `b` and `n` as `const` variables of an appropriate type. Calculate `b` to the power of `n` in a loop and output
  the result.
- Write a function `pot` that receives `b` and `n` and returns the result. Declare it before `main` and define it after
  `main`.
- Use `const` wherever possible.

Extension:

- Formulate the loop once as a `for` and once as a `while` loop.
- For `b=2`: from which `n` on is the result wrong? Why is there no error message? Compare with `unsigned int`.

Review: paste the task and your solution into an LLM. What does it criticize, and is it right?

<hr>

### 👉 Task 'Beverly Hollow'

- Define a `const` number `n` and calculate the first `n` Fibonacci numbers `f_i` iteratively, i.e. in a loop, not
  recursively, and output them. The rule is `f_i = f_(i-1) + f_(i-2)` with `f_0 = 0` and `f_1 = 1`.
- Find the first `i` for which `int` is no longer enough. Switch to `std::uint64_t` from `<cstdint>`: how far do you get
  now?

Extension:

- The session computes `fib` recursively. Count how many calls `fib(30)` needs - with a counter variable defined outside
  the function, e.g. `int calls{0};` right above it, and `++calls;` in the function. Which version is faster, and why?
  Think of the stack frames.

Review: paste the task and your solution into an LLM. What does it criticize, and is it right?

<hr>

### 👉 Task 'Pine Point'

- Write a function `is_prime` that receives a number and returns `true` or `false`.
- Test it for several numbers with `assert` from `<cassert>`, e.g. `assert(is_prime(7));`. A failing `assert` stops the
  program.
- Output all primes below a `const` limit, e.g. 100.

Extension:

- Count the primes below 1'000'000. Measure the runtime in a terminal, once built with `-O0` and once with `-O2` - on
  macOS and Linux with `time ./pinepoint`, on Windows in PowerShell with `Measure-Command { .\pinepoint.exe }`. How big
  is the difference?

Review: paste the task and your solution into an LLM. What does it criticize, and is it right?

<hr>

### 👉 Task 'AI' - Two Opinions

A warm-up for working with AI tools - and for not trusting them blindly. Here are two answers to the question "Does
this C++ program terminate, and what does it print?", as an LLM might write them:

```cpp
#include <iostream>
int main() {
    int n{0};
    for (int i{1}; i > 0; ++i) {
        ++n;
    }
    std::cout << "done, n=" << n << '\n';
}
```

> **Answer A:** Yes, it terminates. `i` counts up to 2147483647, the largest `int`; the next increment wraps around to
> -2147483648, `i > 0` becomes false, and the program prints `done, n=2147483647`. It takes a moment, but it ends.

> **Answer B:** No. Signed overflow is undefined behavior, so the compiler may assume that `i > 0` always holds and turn
> this into an endless loop - the program never prints anything. To fix it, use `long long` instead of `int`; then the
> loop ends correctly.

- Go through both answers statement by statement: which ones are right, which are wrong? Which answer is more useful?
- Run the program yourself, built with `-O0` and with `-O2` (stop it with Ctrl+C if it does not end). Does it confirm
  your judgement? Compare with someone who uses another compiler.
- If you have an LLM at hand, ask it the same question. Is its answer closer to A or to B?

Our results: [answers](../docs/answers.md#ai-0x01).

<hr>

### 👉 Task 'Couch Potato' - Recurring homework

- If you have not completed the tasks in the exercise, complete them at home.
- Work through the required study: all `study_*` files in `iii_follow_up`.
- If you are curious: the `tinker_*` files there.

<hr>

## Comprehension Check

Answers: [comprehension check 0x01](../docs/answers.md#check-0x01).

- I can create, build and run a C++ program, with CMake and with `make`.
- I can define and initialize variables, and I know why we prefer `{}`.
- I can explain what an uninitialized variable contains and why reading it is UB.
- I know that the sizes of types depend on the platform, and how to find them out.
- I know where the characters of a `string` live.
- I know the difference between `s[i]` and `s.at(i)`, and what `string::npos` means.
- I can declare and define functions, and I know what a call does on the stack.
- I can predict what happens on integer overflow.
- I know the control structures `if`, `for`, `while`, `do`-`while` and `switch` - and what a `switch` without `break`
  does.
- I can test with `assert`, and I know why an `assert` must never do work the program needs.
