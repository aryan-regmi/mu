#ifndef MU_SLICE_H
#define MU_SLICE_H

#include "mu/common.h"     // IndexOutOfBounds
#include "mu/primitives.h" // usize, u8< u64
#include <cstring>         // strlen
#include <iostream>        // cout
#include <ostream>         // endl

namespace mu {

namespace internal::helper {
template <typename T>
concept HasDebugFn = requires(const T self) {
  { self.debug() } -> std::same_as<void>;
};
} // namespace internal::helper

// TODO: Add template specialization for make Slice<u8> from const_cstr
//
// TODO: Add Iterator mixin!

/// A dynamically-sized view into a contiguous sequence, [T]. Contiguous here
/// means that elements are laid out so that every element is the same distance
/// from its neighbors.
template <typename T> class Slice {
public:
  explicit Slice() noexcept                     = default;
  ~Slice() noexcept                             = default;
  Slice(const Slice& other) noexcept            = default;
  Slice& operator=(const Slice& other) noexcept = default;

  explicit Slice(T* ptr, usize len, u8 align = alignof(T)) noexcept
      : ptr_{ptr}, len_{len}, align_{align} {}

  /// Returns the number of elements in the slice.
  inline auto len() const noexcept -> usize { return this->len_; }

  /// Returns the underlying pointer the slice points at.
  // inline auto ptr() const noexcept -> T* { return this->ptr_; }
  inline auto ptr() const noexcept -> T* { return this->ptr_; }

  /// Returns the alignment of the slice.
  inline auto align() const noexcept -> u8 { return this->align_; }

  /// Indexes into the slice.
  auto        operator[](u64 idx) -> T& {
    if (idx >= this->len()) {
      throw common::IndexOutOfBounds(idx, this->len());
    }
    return *(this->ptr_ + idx);
  }

  /// Indexes into the slice.
  auto operator[](u64 idx) const -> const T& {
    if (idx >= this->len()) {
      throw common::IndexOutOfBounds(idx, this->len());
    }
    return *(this->ptr_ + idx);
  }

  // TODO: Add `elements` method to get the elements of the slice

  /// Print the slice to `stderr`.
  auto debug() const -> void {
    std::cout << "Slice { ";
    std::cout << "ptr: " << this->ptr_ << ", ";
    std::cout << "len: " << this->len_ << " }";
  }

private:
  T*  ptr_;
  u64 len_ : 56;
  u8  align_ : 8;
};

template <> class Slice<u8> {
public:
  explicit Slice(const_cstr str)
      : ptr_{const_cast<cstr>(str)}, len_{strlen(str)},
        align_{alignof(const_cstr)} {}

  explicit Slice(cstr str)
      : ptr_{str}, len_{strlen(str)}, align_{alignof(cstr)} {}

  /// Returns the number of elements in the slice.
  inline auto len() const noexcept -> usize { return this->len_; }

  /// Returns the underlying pointer the slice points at.
  // inline auto ptr() const noexcept -> T* { return this->ptr_; }
  inline auto ptr() const noexcept -> cstr { return this->ptr_; }

  /// Returns the alignment of the slice.
  inline auto align() const noexcept -> u8 { return this->align_; }

  /// Indexes into the slice.
  auto        operator[](u64 idx) -> char {
    if (idx >= this->len()) {
      throw common::IndexOutOfBounds(idx, this->len());
    }
    return this->ptr_[idx];
  }

  /// Indexes into the slice.
  auto operator[](u64 idx) const -> char {
    if (idx >= this->len()) {
      throw common::IndexOutOfBounds(idx, this->len());
    }
    return this->ptr_[idx];
  }

  /// Print the slice to `stderr`.
  auto debug() const -> void {
    std::cout << "Slice { ";
    std::cout << "ptr: " << this->ptr_ << ", ";
    std::cout << "len: " << this->len_ << " }";
  }

private:
  cstr ptr_;
  u64  len_ : 56;
  u8   align_ : 8;
};

} // namespace mu

#endif // !MU_SLICE_H
