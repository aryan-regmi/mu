#ifndef MU_PRIMITIVES_H
#define MU_PRIMITIVES_H

#include <cstddef> // size_t
#include <cstdint> // uint_least[8-64]_t, int_least[8-64]_t

namespace mu {

typedef size_t         usize;
typedef uint_least8_t  u8;
typedef uint_least16_t u16;
typedef uint_least32_t u32;
typedef uint_least64_t u64;

typedef int_least8_t   i8;
typedef int_least16_t  i16;
typedef int_least32_t  i32;
typedef int_least64_t  i64;

typedef float          f32;
typedef double         f64;

typedef char*          cstr;
typedef const char*    const_cstr;

} // namespace mu

#endif // !MU_MEM_H
