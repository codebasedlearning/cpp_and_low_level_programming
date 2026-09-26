[© A.Voß, FH Aachen, codebasedlearning.dev](mailto:info@codebasedlearning.dev)

# Unit 0x01

## Material

| What | Kind | Time |
|:--|:--|:--|
| `i_preparation`: `a_helloworld` ... `d_var_init` | preparation, before the session | ~45 min |
| `ii_session`: `a_var_init_dive`, `b_var_init_string`, `c_control_flow`, `d_functions` | live session | |
| `tasks.md` | tasks, in a project of your own | ~2 h |
| `iii_follow_up/must_*`: `control_flow`, `string`, `assert` | required study - assumed in tasks and exam | ~30 min |
| `iii_follow_up/may_*`: `int8`, `goto` | optional study - for the curious | as you like |
| `iv_solutions` | one solution per task, after the deadline | |

### todo

## Compile and Run

Open a terminal, change into the `i_preparation` directory, and compile your first C++ file. 
Run

```
g++ a_helloworld.cpp -o a_helloworld.out
```

This produces an executable named `a_helloworld.out`. Now launch it:

```
./a_helloworld.out
```

If you skip the -o option, the compiler defaults to the very imaginative name `a.out` — which 
is fine until you have three of them and can’t remember who’s who.

## Make

Instead of compiling everything every time, use `make`. It checks what changed and only rebuilds 
what’s necessary. The rules and dependencies live in a file called `makefile`. To use it, just type:

```
make
```

## CMake

Behind the scenes in many IDEs (like CLion), `CMake` orchestrates the build. CMake isn’t a build system 
itself — it generates build files for one (e.g., makefiles). In practice, you mostly edit `../CMakeLists.txt`
to declare sources, targets, and dependencies; CLion runs CMake and the build tools for you. 

## Comments

Questions, improvements, or clever suggestions? Send friendly notes (and constructive critique) to
[me](mailto:info@codebasedlearning.dev).


----


/*
* Use of multiple declarators in a single using declaration is possible and a C++17 extension.
*      using std::cout, std::endl;
* A global 'using' directive for the std namespace is also possible. While convenient, it should
* be avoided as it pollutes the global namespace and increases the chance of name collisions.
*      using namespace std;
*/

/*
* The 'main' function serves as the program's entry point and returns an error code.
* A return value of 0 or EXIT_SUCCESS indicates successful execution, while non-zero
* values indicate errors. The error code can be evaluated in command line operations,
* such as 'make && a.out'. The function signature follows the same structure as in Java.
*
*   */

Not guaranteed by C++, but usually double is IEEE 754 binary64.

Whether char is signed or unsigned is implementation-defined. Depending on the compiler and target platform, it behaves as either:

sizeof(char) <= sizeof(short) <= sizeof(int) <= sizeof(long) <= sizeof(long long)

        /* -- .Commonly 64-bit, otherwise use `long long`, signed. -- */

