
# cflow
## compile time junk code injector for kernel and user mode
### works by expanding dead volatile blocks around your real code at compile time so the emitted machine code has no stable shape between builds

# important notes:
- header only single file drop it in and go
- no crt
- no imports
- no allocations
- safe at any irql
- density is tunable per translation unit via obf_density
- every build produces different bytes because the seed chain includes time
- does not stop a determined reverser it just makes reversing it more annoying
# usage
```cpp
#include "obf.hxx"

void some_function() {
    obf_begin

    do_real_work();

    obf_end
}
```

# api

- `obf_salt` override if you want a fixed per project seed instead of per include site
- `obf_density` override per translation unit pick 16 64 256 512 1024
- `obf_rounds` override round count if you expand on it
- `obf_begin` and `obf_end` wrap a block in dead branches the optimizer must keep
- `obf_flatten(block)` wrap a single block under a volatile predicate
- `obf_split(cond, then, else)` two arm flatten for if else chains
- `obf_wrap(expr)` scatters junk around an expression preserving value and type
- `obf_opaque_true()` branchless opaque predicate that is always true
- `obf_opaque_false()` branchless opaque predicate that is always false
- `insert_bogus_block(n)` single dead block seeded by n
- `insert_density_bogus()` expands to obf_density dead blocks
- `insert_heavy_bogus()` four times the chosen density
- `insert_1_bogus` through `insert_1024_bogus` count explicit expansions
- `volatile_sink(v)` forces the compiler to evaluate v and discards the result

# what it does

- every expansion derives its constants from `__COUNTER__` `__LINE__` `obf_salt` and `__TIME__`
(making it in this way prevents an automated script to just remove them from the binary, though this still may be possible) 
- prevents dead code elimination so the compiler cannot remove any of it


# tuning

if you want more noise raise `obf_density` to 512 or 1024 in your tu before including the header

if you want reproducible builds drop the three `__TIME__` terms in `obf_seed` and pin `obf_salt` to a fixed value

if you want less noise drop `obf_density` to 16 or 64
