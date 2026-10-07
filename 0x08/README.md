[© A.Voß, FH Aachen, codebasedlearning.dev](mailto:info@codebasedlearning.dev)

# Unit 0x08 - The Price of Virtual

Virtual costs a pointer per object and a jump per call. This unit follows that pointer. A derived class adds nothing to
an object but its members, and a call the compiler can decide costs what it cost before. `virtual` changes both: every
object gets a hidden pointer to a table of functions - one table per class - and a virtual call takes the address from
there. Cheap, as long as the processor predicts the jump; expensive, when it cannot. The constructors write the
pointer, so an object is a base while its base is being constructed; a copy into a base gets the pointer of the base;
`typeid` and `dynamic_cast` find the type through it. With two bases, an object has two pointers, and a pointer to the
second base is another address. How to write all of this - access, `override`, abstract classes, interfaces, copies of
polymorphic objects - is in the preparation and in the follow-up. The tool of this unit shows what the compiler made of
a class, member by member: the layout dump.

## Comments

Questions, improvements, or clever suggestions? Send friendly notes (and constructive critique) to
[me](mailto:info@codebasedlearning.dev).
