#ifndef MU_ALLOCATOR_H
#define MU_ALLOCATOR_H

#include "mu/primitives.h" // usize, u8
#include "mu/slice.h"      // Slice
#include <cstdint>         // SIZE_MAX
#include <exception>       // exception

namespace mu::mem {
using namespace primitives;

/// Possible `Allocator` errors.
enum class AllocatorError {
  OutOfMemory,
};

/// Exception type thrown by `Allocator` methods.
class AllocatorException : std::exception {
public:
  explicit AllocatorException(AllocatorError type) : type{type} {}

  /// Explains the error
  auto what() const throw() -> const_cstr override;

private:
  AllocatorError type;
};

class Allocator {
public:
  Allocator()          = default;
  virtual ~Allocator() = default;

  /// Allocates `byte_size` bytes of memory aligned to `align`.
  auto                       rawAlloc(usize byte_size, u8 align) -> void*;
  // auto                       rawAlloc(usize byte_size, u8 align) ->
  // NonNull<u8>;

  /// Frees the memory allocated for `ptr`.
  auto                       rawFree(void* ptr, u8 align) noexcept -> void;
  // auto                       rawFree(NonNull<u8> ptr) noexcept -> void;

  /// Allocates and returns memory for a single item of type `T`.
  ///
  /// ## Note
  /// Use `destroy` (*not* `free`) to free the memory allocated by `create`.
  template <typename T> auto create() -> T* {
    Slice<T> slice = allocCustom<T>(1);
    return slice.ptr();
  }

  /// Allocates memory for `len` items of type `T` (returns a slice into the
  /// allocated memory).
  ///
  /// ## Note
  /// Use `free` (*not* `destroy`) to free the memory allocated by `alloc`.
  template <typename T> auto alloc(usize len) -> Slice<T> {
    return allocCustom<T>(len);
  }

  /// Allocates memory for `len` items of type `T` (returns a slice into the
  /// allocated memory), aligned to `align`.
  ///
  /// ## Note
  /// Use `free` (*not* `destroy`) to free the memory allocated by
  /// `allocAligned`.
  template <typename T> auto allocAligned(usize len, u8 align) -> Slice<T> {
    return allocCustom<T>(len, align);
  }

  /// Frees the memory allocated for `ptr`.
  template <typename T> auto destroy(T* ptr) noexcept -> void {
    if (sizeof(T) == 0) {
      return;
    }
    this->rawFree(ptr, alignof(T));
  }

  /// Frees the memory allocated for the `slice`.
  template <typename T> auto free(Slice<T> slice) -> void {
    if ((sizeof(T) == 0) || (slice.len() == 0)) {
      return;
    }
    this->rawFree(slice.ptr(), slice.align());
  }

protected:
  void* ctx = nullptr;

private:
  virtual auto alloc_fn(void* ctx, usize byte_size) noexcept -> void* = 0;
  virtual auto free_fn(void* ctx, void* ptr) noexcept -> void         = 0;

  template <typename T>
  constexpr auto allocCustom(usize len, u8 align = alignof(T)) -> Slice<T> {
    if ((sizeof(T) == 0) || (len == 0)) {
      return Slice(reinterpret_cast<T*>(reinterpret_cast<intptr_t*>(
                       reinterpret_cast<intptr_t>(INTMAX_MAX))),
                   len, align);
    }

    T* ptr = static_cast<T*>(this->rawAlloc(sizeof(T) * len, align));
    if (ptr == nullptr) {
      throw AllocatorException(AllocatorError::OutOfMemory);
    }
    return Slice(ptr, len, align);
  }
};

} // namespace mu::mem

#endif // !MU_ALLOCATOR_H
