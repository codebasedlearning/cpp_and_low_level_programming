[© A.Voß, FH Aachen, codebasedlearning.dev](mailto:info@codebasedlearning.dev)

# Unit 0x05 - Where the Wild Pointers Are

An address is just a number - until you dereference it. This unit follows an address: we compute with it, and `p + 1`
turns out to be the next element, not the next byte; we lose the length of an array to it, and see how C carries the
length of a text in the data, as a `'\0'`; we hand it to functions and to C libraries, and look at the machine code to
see which registers it travels in. And we watch it go wild - null, dangling, out of bounds - and see why a program with
a wild pointer often seems to work.

## Comments

Questions, improvements, or clever suggestions? Send friendly notes (and constructive critique) to
[me](mailto:info@codebasedlearning.dev).
