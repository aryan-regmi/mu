#ifndef MU_SLICE_H
#define MU_SLICE_H

#include "mu/primitives.h"
namespace mu {
using namespace primitives;

// TODO: Impl!
template <typename T> class Slice {
public:
  explicit Slice(T* ptr, usize len) : ptr_{ptr}, len_{len} {}

  inline auto len() const -> usize { return this->len_; }

  inline auto ptr() const -> T* { return this->ptr_; }

private:
  T*    ptr_;
  usize len_;
};

} // namespace mu

#endif // !MU_SLICE_H
