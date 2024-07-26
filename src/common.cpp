#include "mu/common.h"

#include "mu/primitives.h" // usize, u8, const_cstr, cstr
#include <cassert>         // assert
#include <cstdio>          // sprintf

namespace mu::common {

auto IndexOutOfBounds::what() const throw() -> const_cstr {
  return "IndexOutOfBounds: The index must be less than the length";
}

} // namespace mu::common
