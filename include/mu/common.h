#ifndef MU_COMMON_H
#define MU_COMMON_H

#include "mu/primitives.h" // usize, u8
#include <exception>       // exception

namespace mu::common {

/// Exception type thrown by `Allocator` methods.
class IndexOutOfBounds : std::exception {
public:
  explicit IndexOutOfBounds(usize idx, usize len) : idx{idx}, len{len} {}

  /// Explains the error.
  auto  what() const throw() -> const_cstr override;

  usize idx;
  usize len;
};

} // namespace mu::common

#endif // !MU_COMMON_H
