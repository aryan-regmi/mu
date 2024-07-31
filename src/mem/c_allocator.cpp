
#include "mu/mem/c_allocator.h"

#include "mu/mem/allocator.h" // allocator
#include "mu/primitives.h"    // usize, u8

namespace mu::mem {

auto CAllocator::alloc_fn(usize byte_size) noexcept -> void* {
  return std::calloc(1, byte_size);
}

auto CAllocator::free_fn(void* ptr) noexcept -> void { std::free(ptr); }

} // namespace mu::mem
