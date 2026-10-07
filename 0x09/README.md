[© A.Voß, FH Aachen, codebasedlearning.dev](mailto:info@codebasedlearning.dev)

# Unit 0x09 - Functions to Go

A lambda is a struct with an `operator()`. This unit treats functions as values and follows them into memory. A function
is code at an address, and a function pointer holds that address - C hands behavior around this way, and every call
through it is an indirect call. A lambda is an object of a class the compiler writes: its captures are its data members,
a copy lives inside it, a reference is an address it keeps. Every lambda has its own type, so a template sees its body
and inlines it - which is why `std::sort` with a lambda beats `qsort`. `std::function` forgets the type and pays for it:
a size of its own, sometimes an allocation, and a call the compiler cannot look into. And since a lambda is an object,
it can outlive what it captured. How to write all of this - algorithms, lambda rules, callbacks - is in the preparation
and in the follow-up. The tool is an old friend: C++ Insights, which shows the class behind every lambda.

## Comments

Questions, improvements, or clever suggestions? Send friendly notes (and constructive critique) to
[me](mailto:info@codebasedlearning.dev).
