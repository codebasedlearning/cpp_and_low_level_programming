[© A.Voß, FH Aachen, codebasedlearning.dev](mailto:info@codebasedlearning.dev)

# Unit 0x06 - I Like to Move it Move it

Every `new` needs an owner. This unit follows a block of memory on the heap: who asks for it, and what goes wrong when 
nobody knows who deletes - a leak, a double `delete`, a copy that shares its owner. Then the owner becomes a type:
`unique_ptr`, as small as a raw pointer, and with the same machine code. Ownership is handed on by a move, which steals
the pointer instead of copying the elements - and shared by `shared_ptr`, which counts its owners. 

## Comments

Questions, improvements, or clever suggestions? Send friendly notes (and constructive critique) to
[me](mailto:info@codebasedlearning.dev).
