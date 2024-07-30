#ifndef MU_C_ALLOCATOR_H
#define MU_C_ALLOCATOR_H

#include "mu/mem/allocator.h" // Allocator
#include "mu/primitives.h"    // usize, u8

#include <cstdlib> // malloc, free

namespace mu::mem {
using namespace primitives;

class CAllocator : public Allocator {
public:
  CAllocator()                                   = default;
  ~CAllocator()                                  = default;
  CAllocator(const CAllocator& other)            = default;
  CAllocator(CAllocator&& other)                 = default;
  CAllocator& operator=(const CAllocator& other) = default;
  CAllocator& operator=(CAllocator&& other)      = default;

private:
  auto alloc_fn(usize byte_size) noexcept -> void* override {
    return std::calloc(1, byte_size);
  }

  auto free_fn(void* ptr) noexcept -> void override { std::free(ptr); }
};

} // namespace mu::mem

#endif // !MU_C_ALLOCATOR_H
