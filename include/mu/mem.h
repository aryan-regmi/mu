#ifndef MU_MEM_H
#define MU_MEM_H

#include "mu/primitives.h" // usize, u8
#include "mu/result.h"     // Result

#include <cstdint>     // SIZE_MAX
#include <type_traits> // is_array_v
#include <utility>     // move

namespace mu::mem {
using namespace primitives;

typedef void* (*AllocFn)(void* ctx, usize byte_size, u8 align);

typedef void* (*ResizeFn)(void* ctx, void* ptr, usize new_byte_szie, u8 align);

typedef void (*FreeFn)(void* ctx, void* ptr, u8 align);

// TODO: Check that `align` is a power of 2!
class Allocator {
public:
  enum class AllocatorError {
    OutOfMemory,
  };

  // Impl from:
  // https://johanmabille.github.io/blog/2014/12/06/aligned-memory-allocator/
  auto rawAlloc(usize byte_size, u8 align) -> void* {
    void* res = nullptr;
    void* ptr = this->alloc_fn(this->ctx, byte_size, align);
    if (ptr != nullptr) {
      res = reinterpret_cast<void*>(
          (reinterpret_cast<size_t>(ptr) & ~(size_t(align - 1))) + align);
      *(reinterpret_cast<void**>(res) - 1) = ptr;
    }
    return res;
  }

  auto rawResize(void* ptr, usize new_byte_szie, u8 align) -> void*;

  auto rawFree(void* ptr, u8 align) -> void;

  template <typename T> auto create() -> Result<T*, AllocatorError> {
    if (sizeof(T) == 0) {
      return Ok(reinterpret_cast<T*>(
          reinterpret_cast<intptr_t*>(reinterpret_cast<intptr_t>(INTMAX_MAX))));
    }

    T* ptr = static_cast<T*>(this->rawAlloc(sizeof(T), alignof(T)));
    if (ptr == nullptr) {
      return Err(AllocatorError::OutOfMemory);
    }
    return Ok(ptr);
  }

  // template <typename T>
  // auto destroy(T* ptr) -> void
  //   requires(!std::is_array_v<T*>)
  // {
  //   if (sizeof(T) == 0) {
  //     return;
  //   }
  //   // TODO: Impl
  //
  //   // this->rawFree(ptr, std::log2(alignof(T)));
  // }

protected:
  void*    ctx;
  AllocFn  alloc_fn;
  ResizeFn resize_fn;
  FreeFn   free_fn;
};

} // namespace mu::mem

#endif // !MU_MEM_H
