#ifndef MU_SLICE_H
#define MU_SLICE_H

#include "mu/primitives.h"
namespace mu {
using namespace primitives;

// TODO: Impl!
template <typename T> class Slice {
public:
  explicit Slice(T* ptr, usize len, u8 align = alignof(T))
      : ptr_{ptr}, len_{len}, align_{align} {}

  inline auto len() const -> usize { return this->len_; }

  inline auto ptr() const -> T* { return this->ptr_; }

  inline auto align() const -> u8 { return this->align_; }

private:
  T*  ptr_;
  u64 len_ : 56;
  u8  align_ : 8;
};

} // namespace mu

#endif // !MU_SLICE_H
