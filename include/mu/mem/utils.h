#ifndef MU_MEM_H
#define MU_MEM_H

#include "mu/primitives.h"
#include <algorithm>
#include <array>

namespace mu::mem {
using namespace primitives;
// NOTE: Impl from:
// https://mklimenko.github.io/english/2018/08/22/robust-endian-swap/
//
/// Swaps the endian-ness of the passed object/value.
template <typename T> auto swapEndian(T& val) {
  union U {
    T                         val;
    std::array<u8, sizeof(T)> raw;
  } src, dst;

  src.val = val;
  std::reverse_copy(src.raw.begin(), src.raw.end(), dst.raw.begin());
  val = dst.val;
}

} // namespace mu::mem

#endif // !MU_MEM_H
