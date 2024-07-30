#ifndef MU_BUFFER_H
#define MU_BUFFER_H

#include "mu/primitives.h" // u8, usize, cstr
#include <cstring>         // strlen

namespace mu::mem {
using namespace primitives;

struct Buffer {
  explicit Buffer() = default;

  explicit Buffer(u8* buf, usize len) : ptr{buf}, len{len} {}

  /// Create a buffer from the given string.
  explicit Buffer(cstr str)
      : ptr{reinterpret_cast<u8*>(str)}, len{std::strlen(str)} {}

  explicit Buffer(const_cstr str)
      : ptr{reinterpret_cast<u8*>(const_cast<cstr>(str))},
        len{std::strlen(str)} {}

  // TODO: Impl!

  u8*   ptr = nullptr;
  usize len = 0;
};

} // namespace mu::mem

#endif // !MU_BUFFER_H
