#include "mu/mem/allocator.h"

#include "mu/primitives.h" // usize, u8

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
  u8    offset      = align - 1;
  u16   header_size = sizeof(align) + (align - 1);

  void* aligned     = nullptr;
  void* ptr         = this->alloc_fn(this->ctx, byte_size + header_size, align);
  if (ptr != nullptr) {
    void* aligned = reinterpret_cast<void*>(
        (reinterpret_cast<usize>(ptr) + offset) & ~(offset));
    *(reinterpret_cast<usize*>(aligned) - 1) =
        reinterpret_cast<usize>(aligned) - reinterpret_cast<usize>(ptr);
  }
  return aligned;

  // void* res = nullptr;
  // // void* ptr = this->alloc_fn(this->ctx, byte_size, align);
  // void* ptr = this->alloc_fn(this->ctx, byte_size, align);
  // if (ptr != nullptr) {
  //   res = reinterpret_cast<void*>(
  //       (reinterpret_cast<size_t>(ptr) & ~(size_t(align - 1))) + align);
  //   *(reinterpret_cast<void**>(res) - 1) = ptr;
  // }
  // return res;
}

auto Allocator::rawFree(void* ptr) noexcept -> void {
  if (ptr != 0) {
    this->free_fn(this->ctx, *(reinterpret_cast<void**>(ptr) - 1));
  }
}

} // namespace mu::mem
