#ifndef MU_SLICE_H
#define MU_SLICE_H

#include "mu/common.h"     // IndexOutOfBounds
#include "mu/primitives.h" // usize, u8< u64

namespace mu {
using namespace primitives;

/// A dynamically-sized view into a contiguous sequence, [T]. Contiguous here
/// means that elements are laid out so that every element is the same distance
/// from its neighbors.
template <typename T> class Slice {
public:
  explicit Slice(T* ptr, usize len, u8 align = alignof(T)) noexcept
      : ptr_{ptr}, len_{len}, align_{align} {}

  /// Returns the number of elements in the slice.
  inline auto len() const noexcept -> usize { return this->len_; }

  /// Returns the underlying pointer the slice points at.
  inline auto ptr() const noexcept -> T* { return this->ptr_; }

  /// Returns the alignment of the slice.
  inline auto align() const noexcept -> u8 { return this->align_; }

  /// Indexes into the slice.
  T&          operator[](u64 idx) {
    if (idx >= this->len()) {
      throw common::IndexOutOfBounds(idx, this->len());
    }
    return *reinterpret_cast<T*>(reinterpret_cast<u8*>(this->ptr_) + idx);
  }

private:
  T*  ptr_;
  u64 len_ : 56;
  u8  align_ : 8;
};

} // namespace mu

#endif // !MU_SLICE_H
