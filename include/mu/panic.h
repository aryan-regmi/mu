#ifndef MU_PANIC_H
#define MU_PANIC_H

#include "mu/primitives.h" // usize, const_cstr
#include <cassert>         // assert
#include <cstdio>          // fprintf, stderr
#include <cstdlib>         // abort

namespace mu {
using namespace primitives;

// TODO: Add ability to change panic handler

inline auto defaultPanicHandler(const_cstr file, usize line,
                                const_cstr msg) -> void {
  int written = std::fprintf(stderr, "[PANIC] %s  at %s:%zu", msg, file, line);
  assert(written != 0);
  abort();
}

#define MU_PANIC(msg) defaultPanicHandler(__FILE__, __LINE__, msg)

} // namespace mu

#endif // !MU_PANIC_H
