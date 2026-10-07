[© A.Voß, FH Aachen, codebasedlearning.dev](mailto:info@codebasedlearning.dev)

# Unit 0x0a - Many Hands

Nothing is atomic unless you say so. This unit follows a shared variable through several threads. A thread is a stack of
its own and a set of registers; everything else - the globals, the heap, the code - it shares with all the others.
`++counter` is a load, an add and a store, and two cores can interleave them: updates get lost, and since a data race is
undefined behavior, the compiler may even make it look right - or remove a waiting loop altogether. An atomic makes the
three steps one, with a `lock` on x86-64 and `ldadd` on ARM64; a mutex costs one atomic instruction while nobody waits,
and a system call when somebody does. Underneath it all, the cores share memory by cache lines of 64 bytes, and two
threads that write to the same line slow each other down - even when they never touch the same variable. How to write
all of this - threads, locks, condition variables, futures - is in the preparation and in the follow-up. The tool is the
debugger, with all threads at once; ThreadSanitizer waits in the follow-up, where the platform allows.

## Comments

Questions, improvements, or clever suggestions? Send friendly notes (and constructive critique) to
[me](mailto:info@codebasedlearning.dev).
