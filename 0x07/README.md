[© A.Voß, FH Aachen, codebasedlearning.dev](mailto:info@codebasedlearning.dev)

# Unit 0x07 - Sugar Coated

An operator is just a function with a funny name. This unit takes the sugar off the syntax and looks at what is left
of it in the machine: a call, an instruction, or nothing at all. `a + b` for a class of your own is a call - and as
Release, the same instruction as for two numbers. A cast either computes a new value or only changes how the compiler
reads the bits, and a `const_cast` does not make a constant writable. A `static` member is not in the object but next
to the global variables, and a `static` local variable pays for its first initialization at every call. An enum is a
number with names, and a `switch` over it becomes a table. How to write all of this - which operator as a member,
which cast for what, enums, streams and files - is in the preparation and in the follow-up. The tool of this unit
shows the code the way the compiler sees it: C++ Insights.

## Comments

Questions, improvements, or clever suggestions? Send friendly notes (and constructive critique) to
[me](mailto:info@codebasedlearning.dev).
