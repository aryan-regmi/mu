#include "mu/mem/allocator.h"

#include "mu/primitives.h" // usize, u8
#include <cstdio>

namespace mu::mem {

auto AllocatorException::what() const throw() -> const_cstr {
  switch (this->type) {
  case AllocatorError::OutOfMemory:
    return "OutOfMemory: Not enough memory available for the allocation";
  default:
    return nullptr;
  }
}

// FIXME: Check that `align` is a power of 2!
// NOTE: Impl from:
// https://johanmabille.github.io/blog/2014/12/06/aligned-memory-allocator/
auto Allocator::rawAlloc(usize byte_size, u8 align) -> void* {
  u8*   res = nullptr;
  void* ptr = this->alloc_fn(this->ctx, byte_size + align);
  if (ptr != nullptr) {
    // Align allocation
    res = reinterpret_cast<u8*>(
        (reinterpret_cast<usize>(ptr) & ~(usize(align - 1))) + align);

    // Set offset
    u8 offset      = res - reinterpret_cast<u8*>(ptr);
    *(res - align) = offset;
  }
  return res;
}

auto Allocator::rawFree(void* ptr, u8 align) noexcept -> void {
  if (ptr != nullptr) {
    u8* aligned = reinterpret_cast<u8*>(ptr);
    u8* offset  = aligned - align;
    u8* alloced = aligned - *offset;
    this->free_fn(this->ctx, alloced);
  }
}

} // namespace mu::mem
