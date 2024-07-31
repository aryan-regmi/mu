#ifndef MU_UNIQUE_PTR_H
#define MU_UNIQUE_PTR_H

#include "mu/mem/allocator.h"
#include <algorithm>
#include <memory>
#include <utility>
namespace mu {

// TODO: Specialize for arrays/slices
template <typename T> class UniquePtr {
public:
  UniquePtr(const UniquePtr&)                    = delete;
  auto operator=(const UniquePtr&) -> UniquePtr& = delete;

  template <typename... Args>
  static auto create(mem::Allocator& allocator, Args... args) -> UniquePtr<T> {
    T* data = allocator.create<T>();
    *data   = T{std::forward<Args>(args)...};
    return UniquePtr(allocator, data);
  }

  explicit UniquePtr(mem::Allocator& allocator, T* data)
      : allocator{allocator}, data{data} {}

  UniquePtr(UniquePtr&& other) noexcept {
    this->allocator = other.allocator;
    this->data      = other.data;
    other.data      = nullptr;
  }

  UniquePtr& operator=(UniquePtr&& other) noexcept {
    if (this->data == other.data) {
      return *this;
    }
    this->allocator = other.allocator;
    this->data      = other.data;
    other.data      = nullptr;
  }

  ~UniquePtr() { this->allocator.destroy(this->data); }

  constexpr auto     operator->() const noexcept -> T* { return data; }

  constexpr auto     operator*() const noexcept -> T& { return *data; }

  constexpr explicit operator bool() const noexcept { return data; }

  /// Get the data stored in the `UniquePtr`.
  auto               get() const -> T* { return data; }

  /// Returns a pointer to the managed object and releases the ownership
  auto               release() noexcept -> T* {
    T* res = nullptr;
    std::swap(res, this->data);
    return res;
  }

  // Replaces the current pointer by `ptr`.
  //
  // ## Note
  // `ptr` must be allocated using the same allocator as `this`.
  auto replace(T* ptr) noexcept -> void {
    T* old     = this->data;
    this->data = ptr;
    if (old) {
      this->allocator.destroy(old);
    }
  }

private:
  mem::Allocator& allocator;
  T*              data;
};

} // namespace mu

#endif // !MU_UNIQUE_PTR_H
