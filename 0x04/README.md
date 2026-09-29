[© A.Voß, FH Aachen, codebasedlearning.dev](mailto:info@codebasedlearning.dev)

# Unit 0x04 - Just Looking

A view is two words: where, and how long. This unit is about looking at data without owning it - a `string_view` into
characters, a `span` or a pair of iterators into elements - and about what that means in memory: no copy, 16 bytes,
and nobody who keeps the data alive. Then we look into the object files themselves: a template generates code per
type, `nm` shows which functions end up where, and the linker decides which copy survives. On the way, our classes
learn to print themselves with `operator<<`.

## Comments

Questions, improvements, or clever suggestions? Send friendly notes (and constructive critique) to
[me](mailto:info@codebasedlearning.dev).
