#pragma once
#include <ntddk.h>
#include <impl/obf/obf.hxx>

#ifndef obf_salt
#define obf_salt ((unsigned)__LINE__)
#endif

#ifndef obf_rounds
#define obf_rounds ((unsigned)__LINE__)
#endif

#ifndef obf_density
#define obf_density 256
#endif


#define obf_seed(n) (                                       \
    ((unsigned)(n))                                         \
    ^ ((unsigned)(__LINE__))                                \
    ^ ((unsigned)(__COUNTER__))                             \
    ^ ((unsigned)(obf_salt))                                \
    ^ ((unsigned)(__TIME__[0]))                             \
    ^ ((unsigned)(__TIME__[1]))                             \
    ^ ((unsigned)(__TIME__[2]))                             \
)

#define volatile_sink(v) do {                               \
    volatile unsigned __vs = (unsigned)(v);                 \
    __vs ^= __vs << 1;                                      \
    __vs += __vs >> 1;                                      \
    __vs *= __vs | 1u;                                      \
    __vs ^= __vs >> (__vs & 31u);                           \
    (void)__vs;                                             \
} while(0)


#define insert_bogus_block(n) do {                          \
    unsigned __a = obf_seed(n);                             \
    unsigned __b = obf_seed((n) ^ __a);                     \
    __a ^= __b + obf_seed(__LINE__);                        \
    __a = (__a << (__a & 31u)) | (__a >> ((32u - __a) & 31u)); \
    __b ^= __a * obf_seed(__COUNTER__);                     \
    __a += __b ^ obf_seed(__a);                             \
    volatile_sink(__a);                                     \
    volatile_sink(__b);                                     \
} while(0)

#define insert_1_bogus()    insert_bogus_block(__COUNTER__)
#define insert_2_bogus()    insert_1_bogus();  insert_1_bogus()
#define insert_4_bogus()    insert_2_bogus();  insert_2_bogus()
#define insert_8_bogus()    insert_4_bogus();  insert_4_bogus()
#define insert_16_bogus()   insert_8_bogus();  insert_8_bogus()
#define insert_32_bogus()   insert_16_bogus(); insert_16_bogus()
#define insert_64_bogus()   insert_32_bogus(); insert_32_bogus()
#define insert_128_bogus()  insert_64_bogus(); insert_64_bogus()
#define insert_256_bogus()  insert_128_bogus(); insert_128_bogus()
#define insert_512_bogus()  insert_256_bogus(); insert_256_bogus()
#define insert_1024_bogus() insert_512_bogus(); insert_512_bogus()

#define insert_density_bogus()   insert_density_pick(obf_density)
#define insert_density_pick(d)   insert_density_pick_(d)
#define insert_density_pick_(d)  insert_density_pick__(d)
#define insert_density_pick__(d) insert_##d##_bogus()

#define obf_flatten(block) do {                             \
    volatile unsigned __pred = obf_seed(__COUNTER__);       \
    if (__pred | 1u) { block }                              \
    else { volatile_sink(__pred); }                         \
} while(0)

#define obf_opaque_true()  (obf_seed(__COUNTER__) | (unsigned)__LINE__ | 1u)

#define obf_opaque_false() (0u & obf_seed(__COUNTER__) & (unsigned)__LINE__)

#define obf_begin do {                                      \
    insert_density_bogus();                                 \
    if (obf_opaque_true()) {

#define obf_end                                             \
    }                                                       \
    else { volatile_sink(obf_seed(__COUNTER__)); }          \
    insert_density_bogus();                                 \
} while(0)

#define obf_split(cond, then_block, else_block) do {        \
    insert_density_bogus();                                 \
    volatile unsigned __c = (unsigned)(cond);               \
    if (__c ^ (obf_seed(__COUNTER__) & 0u)) { then_block }  \
    else { else_block }                                     \
    insert_density_bogus();                                 \
} while(0)

#define obf_wrap(expr) (                                    \
    []() -> decltype(expr) {                                \
        insert_density_bogus();                             \
        decltype(expr) __r = (expr);                        \
        insert_density_bogus();                             \
        volatile_sink(obf_seed(__COUNTER__));               \
        return __r;                                         \
    }()                                                     \
)

#define insert_heavy_bogus() do {                           \
    insert_density_bogus();                                 \
    insert_density_bogus();                                 \
    insert_density_bogus();                                 \
    insert_density_bogus();                                 \
} while(0)

#define obfuscate_begin obf_begin
#define obfuscate_end   obf_end
