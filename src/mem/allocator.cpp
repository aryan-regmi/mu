#include "mu/mem/allocator.h"

#include "mu/primitives.h" // usize, u8
#include <cassert>         // assert

namespace mu::mem {

constexpr auto isPowerOf2(u8 val) noexcept -> bool {
  return (val != 0) && ((val & (val - 1)) == 0);
  ;
}

auto AllocatorException::what() const throw() -> const_cstr {
  switch (this->type) {
  case AllocatorError::OutOfMemory:
    return "OutOfMemory: Not enough memory available for the allocation";
  default:
    return nullptr;
  }
}

// NOTE: Impl from:
// https://johanmabille.github.io/blog/2014/12/06/aligned-memory-allocator/
auto Allocator::rawAlloc(usize byte_size, u8 align) -> void* {
  assert(isPowerOf2(align));
  u8*   res = nullptr;
  void* ptr = this->alloc_fn(byte_size + align);
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
    this->free_fn(alloced);
  }
}

} // namespace mu::mem
