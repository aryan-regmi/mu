#ifndef MU_UNIQUE_PTR_H
#define MU_UNIQUE_PTR_H

#include "mu/cloneable.h"
#include "mu/mem/allocator.h"   // Allocator
#include "mu/mem/c_allocator.h" // CAllocator
#include "mu/primitives.h"      // usize, u64
#include "mu/slice.h"           // Slice
#include <type_traits>          // is_same_v
#include <utility>              // forward, swap

namespace mu {

namespace internal::helper {

/// TODO: Replace with check for static allocator instead!!
///   - Must be default constructible
///   - Must inherit base type `StaticAllocator`
///   - Move this to `allocator.h` instead!
template <typename T>
concept IsCAllocator = std::is_same_v<T, mem::CAllocator>;
} // namespace internal::helper

/// A smart pointer that owns and manages another object through a pointer and
/// disposes of that object when the `UniquePtr` goes out of scope.
///
/// # Note
/// Manages a single object of type `T`.
template <typename T, class Allocator = mem::CAllocator> class UniquePtr {
  struct empty {};

public:
  UniquePtr(const UniquePtr&)                    = delete;
  auto operator=(const UniquePtr&) -> UniquePtr& = delete;

  /// Constructs an object of type `T` and wraps it in a `UniquePtr`.
  template <typename... Args>
  static auto create(mem::Allocator* allocator = nullptr,
                     Args... args) -> UniquePtr<T> {
    T* data;
    if constexpr (internal::helper::IsCAllocator<Allocator>) {
      data  = mem::CAllocator().create<T>();
      *data = T{std::forward<Args>(args)...};
      return UniquePtr(empty{}, data);
    } else {
      data  = allocator->create<T>();
      *data = T{std::forward<Args>(args)...};
      return UniquePtr(allocator, data);
    }
  }

  /// Creates a `UniquePtr` from a pointer (`data`) allocated using `allocator`.
  explicit UniquePtr(mem::Allocator* allocator, T* data)
      : allocator{allocator}, data{data} {}

  // FIXME: Make private
  explicit UniquePtr(empty allocator, T* data)
      : allocator{allocator}, data{data} {}

  /// Creates a `UniquePtr` by transferring ownership from `other` to `this` and
  /// stores the null pointer in `other`.
  UniquePtr(UniquePtr&& other) noexcept {
    this->allocator = std::move(other.allocator);
    this->data      = other.data;
    other.data      = nullptr;
  }

  /// Move assignment operator.
  /// Transfers ownership from `other` to `this`.
  auto operator=(UniquePtr&& other) noexcept -> UniquePtr& {
    if (this->data == other.data) {
      return *this;
    }
    // TODO: Free old data?
    this->allocator = other.allocator;
    this->data      = other.data;
    other.data      = nullptr;
  }

  /// Destroys the managed object.
  ~UniquePtr() {
    if constexpr (internal::helper::IsCAllocator<Allocator>) {
      mem::CAllocator().destroy(this->data);
    } else {
      this->allocator->destroy(this->data);
    }
  }

  ///  Returns the object owned by `this`.
  constexpr auto     operator*() const noexcept -> T& { return *data; }

  ///  Returns a pointer to the object owned by `this`.
  constexpr auto     operator->() const noexcept -> T* { return data; }

  /// Checks if `this` owns an object.
  ///
  /// Returns `true` if `this.data != nullptr`.
  constexpr explicit operator bool() const noexcept { return data; }

  /// Checks if `this` owns an object.
  ///
  /// Returns `true` if `this.data != nullptr`.
  auto               valid() const -> bool { return data != nullptr; }

  /// Get the data stored in the `UniquePtr`.
  auto               get() const -> T* { return data; }

  /// Returns a pointer to the managed object and releases the ownership.
  [[nodiscard]] auto release() noexcept -> T* {
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
      this->allocator->destroy(old);
    }
  }

  // TODO: derive for `Cloneable<T>` or `Copyable<T>`
  //  - have a constexpr if to check for each
  //
  /// Clones the contained value.
  auto clone() const -> UniquePtr<T>
    requires(Cloneable<T>)
  {
    if constexpr (internal::helper::IsCAllocator<Allocator>) {
      T* data = mem::CAllocator().template create<T>();
      *data   = this->data->clone();
      return UniquePtr(empty{}, data);
    } else {
      T* data = this->allocator->template create<T>();
      *data   = this->data->clone();
      return UniquePtr(this->allocator, data);
    }
  }

private:
  using AllocatorType =
      std::conditional_t<std::is_same_v<Allocator, mem::CAllocator>, empty,
                         Allocator*>;
  [[no_unique_address]] AllocatorType allocator;
  T*                                  data;
};

/// A smart pointer that owns and manages another object through a pointer and
/// disposes of that object when the `UniquePtr` goes out of scope.
///
/// # Note
/// Manages a dynamically-allocated array of objects.
template <typename T> class UniquePtr<Slice<T>> {
public:
  UniquePtr(const UniquePtr&)                    = delete;
  auto operator=(const UniquePtr&) -> UniquePtr& = delete;

  /// Constructs `len` number of objects of type `T` and wraps the resulting
  /// slice in a `UniquePtr`.
  template <typename... Args>
  static auto create(mem::Allocator* allocator, usize len,
                     Args... args) -> UniquePtr<Slice<T>> {
    Slice<T> data = allocator->alloc<T>(len);
    for (usize i = 0; i < len; i++) {
      data[i] = T{std::forward<Args>(args)...};
    }
    return UniquePtr(allocator, data);
  }

  /// Creates a `UniquePtr` from a slice (`data`) allocated using `allocator`.
  explicit UniquePtr(mem::Allocator* allocator, Slice<T> data)
      : allocator{allocator}, data{data} {}

  /// Creates a `UniquePtr` by transferring ownership from `other` to `this`.
  UniquePtr(UniquePtr&& other) noexcept {
    this->allocator = other.allocator;
    this->data      = other.data;
    other.data      = Slice<T>(nullptr, 0);
  }

  /// Move assignment operator.
  /// Transfers ownership from `other` to `this`.
  auto operator=(UniquePtr&& other) noexcept -> UniquePtr& {
    if (this->data.ptr() == other.data.ptr()) {
      return *this;
    }
    // TODO: Free old data?
    this->allocator = other.allocator;
    this->data      = other.data;
    other.data      = Slice<T>(nullptr, 0);
  }

  /// Destroys the managed slice.
  ~UniquePtr() { this->allocator->free(this->data); }

  ///  Returns the slice owned by `this`.
  constexpr auto     operator*() const noexcept -> Slice<T>& { return data; }

  ///  Returns the slice owned by `this`.
  constexpr auto     operator->() const noexcept -> Slice<T>& { return data; }

  /// Checks if `this` owns a slice.
  ///
  /// Returns `true` if `this.data` contains a valid slice.
  constexpr explicit operator bool() const noexcept { return data.ptr(); }

  /// Indexes into the underlying slice.
  auto               operator[](u64 idx) -> T& { return this->data[idx]; }

  /// Indexes into the underlying slice.
  auto operator[](u64 idx) const -> const T& { return this->data[idx]; }

  /// Checks if `this` owns a slice.
  ///
  /// Returns `true` if `this.data` contains a valid slice.
  auto valid() const -> bool { return data.ptr() != nullptr; }

  /// Get the slice stored in the `UniquePtr`.
  auto get() const -> Slice<T> { return data; }

  /// Returns the managed slice and releases the ownership.
  auto release() noexcept -> Slice<T> {
    Slice<T> res{};
    std::swap(res, this->data);
    return res;
  }

  // Replaces the current slice by `slice`.
  //
  // ## Note
  // `slice` must be allocated using the same allocator as `this`.
  auto replace(Slice<T> slice) noexcept -> void {
    Slice<T> old = this->data;
    this->data   = slice;
    if (old.ptr()) {
      this->allocator->free(old);
    }
  }

  // TODO: derive for `Cloneable<T>` or `Copyable<T>`
  //  - have a constexpr if to check for each
  //
  /// Clones the contained value.
  auto clone() const -> UniquePtr<Slice<T>>
    requires(Cloneable<T>)
  {
    Slice<T> data = this->allocator->template alloc<T>(this->data.len());
    for (usize i = 0; i < this->data.len(); i++) {
      data[i] = this->data[i].clone();
    }
    return UniquePtr(this->allocator, data);
  }

private:
  mem::Allocator* allocator;
  Slice<T>        data;
};

} // namespace mu

#endif // !MU_UNIQUE_PTR_H
