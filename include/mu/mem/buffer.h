#ifndef MU_BUFFER_H
#define MU_BUFFER_H

#include "mu/mem/allocator.h" // Allocator
#include "mu/primitives.h"    // usize

namespace mu::mem {
class Buffer {
public:
  explicit Buffer(Allocator allocator, usize capacity = 0)
      : allocator{allocator}, capacity{capacity} {
    if (capacity == 0) {
      this->len = 0;
      this->buf = nullptr;
      return;
    }

    // u8* buf = allocator.rawAlloc(, u8 align);

    // TODO: Impl
  }

private:
  mem::Allocator allocator;
  usize          len;
  usize          capacity;
  u8*            buf;
};

} // namespace mu::mem

#endif // !MU_BUFFER_H
