#ifndef MU_SLICE_H
#define MU_SLICE_H

#include "mu/common.h"     // IndexOutOfBounds
#include "mu/primitives.h" // usize, u8< u64
#include <iostream>
#include <ostream>
#include <source_location>
#include <type_traits>

namespace mu {
using namespace primitives;

namespace internal::helper {
template <typename T>
concept HasDebugFn = requires(const T self) {
  { self.debug() } -> std::same_as<void>;
};
} // namespace internal::helper

// TODO: Move to impl file.
//
/// A dynamically-sized view into a contiguous sequence, [T]. Contiguous here
/// means that elements are laid out so that every element is the same distance
/// from its neighbors.
template <typename T> class Slice {
public:
  explicit Slice()                     = default;
  ~Slice()                             = default;
  Slice(const Slice& other)            = default;
  Slice& operator=(const Slice& other) = default;

  explicit Slice(T* ptr, usize len, u8 align = alignof(T)) noexcept
      : ptr_{ptr}, len_{len}, align_{align} {}

  // CAllocator& operator=(const CAllocator& other) = default;

  /// Returns the number of elements in the slice.
  inline auto len() const noexcept -> usize { return this->len_; }

  /// Returns the underlying pointer the slice points at.
  // inline auto ptr() const noexcept -> T* { return this->ptr_; }
  inline auto ptr() const noexcept -> T* { return this->ptr_; }

  /// Returns the alignment of the slice.
  inline auto align() const noexcept -> u8 { return this->align_; }

  /// Indexes into the slice.
  T&          operator[](u64 idx) {
    if (idx >= this->len()) {
      throw common::IndexOutOfBounds(idx, this->len());
    }
    return *(this->ptr_ + idx);
  }

  /// Print the slice to `stderr`.
  auto debug(const std::source_location loc =
                 std::source_location::current()) const -> void {
    std::cout << "[" << loc.file_name() << ":" << loc.line() << ":"
              << loc.column() << "] = ";
    std::cout << "Slice {" << std::endl;
    std::cout << "\tptr: " << this->ptr_ << std::endl;
    std::cout << "\tlen: " << this->len_ << std::endl;
    std::cout << "\telements: [";
    for (usize i = 0; i < this->len_; i++) {
      if constexpr (internal::helper::HasDebugFn<T>) {
        // TODO: Call T debug!!
      } else {
        std::cout << " " << *(this->ptr_ + i);
      }
      if (i < this->len_ - 1) {
        std::cout << ",";
      }
    }
    std::cout << " ]" << std::endl;
    std::cout << "}" << std::endl;
  }

private:
  T*  ptr_;
  u64 len_ : 56;
  u8  align_ : 8;
};

} // namespace mu

#endif // !MU_SLICE_H
