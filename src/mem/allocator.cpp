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
  void* res = nullptr;
  void* ptr = this->alloc_fn(this->ctx, byte_size + align);
  if (ptr != nullptr) {
    res = reinterpret_cast<void*>(
        (reinterpret_cast<usize>(ptr) & ~(usize(align - 1))) + align);
    *(reinterpret_cast<void**>(res) - 1) = ptr;
    // printf("\nPTR: %p, RES: %p, HDR: %p\n", ptr, res,
    //        *(reinterpret_cast<void**>(res) - 1)); // FIX: REMOVE
  }
  return res;
}

auto Allocator::rawFree(void* ptr) noexcept -> void {
  if (ptr != nullptr) {
    // usize offset = *(reinterpret_cast<usize*>(ptr) - 1);
    // this->free_fn(this->ctx, reinterpret_cast<void*>(
    //                              *(reinterpret_cast<usize*>(ptr)) - offset));

    this->free_fn(this->ctx, *(reinterpret_cast<void**>(ptr) - 1));
  }
}

} // namespace mu::mem
