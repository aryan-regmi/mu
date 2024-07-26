#ifndef MU_SLICE_H
#define MU_SLICE_H

#include "mu/primitives.h"
namespace mu {
using namespace primitives;

/// A dynamically-sized view into a contiguous sequence, [T]. Contiguous here
/// means that elements are laid out so that every element is the same distance
/// from its neighbors.
template <typename T> class Slice {
public:
  explicit Slice(T* ptr, usize len, u8 align = alignof(T)) noexcept
      : ptr_{ptr}, len_{len}, align_{align} {}

  inline auto len() const noexcept -> usize { return this->len_; }

  inline auto ptr() const noexcept -> T* { return this->ptr_; }

  inline auto align() const noexcept -> u8 { return this->align_; }

  T&          operator[](u64 idx) {
    if (idx >= this->len()) {
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
