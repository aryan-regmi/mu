#include "mu/common.h"

#include "mu/primitives.h" // usize, u8, const_cstr, cstr
#include <cassert>         // assert
#include <cstdio>          // sprintf

namespace mu::common {

auto IndexOutOfBounds::what() const throw() -> const_cstr {
  return "IndexOutOfBounds: The index must be less than the length";
}

auto OptionUnwrapException::what() const throw() -> const_cstr {
  return "OptionUnwrapException: Called `unwrap` on an empty optional; use "
         "`isValid` to check if the optional value exists first";
}

auto ResultUnwrapErrException::what() const throw() -> const_cstr {
  return "ResultUnwrapErrException: Called `unwrap` on result containing an "
         "`Err`; use `isOk` to check if the result is an `Ok` type first";
}

auto ResultUnwrapOkException::what() const throw() -> const_cstr {
  return "ResultUnwrapOkException: Called `unwrapErr` on result containing "
         "an "
         "`Ok`; use `isErr` to check if the result is an `Err` type first";
}

auto OutOfMemoryException::what() const throw() -> const_cstr {
  const_cstr str     = reinterpret_cast<const_cstr>(this->buf);
  usize      written = sprintf(const_cast<cstr>(str),
                               "OutOfMemory: Not enough memory available for the "
                                    "allocation of %zu bytes",
                               this->allocation_size);
  assert(written != 0);
  return str;
}

} // namespace mu::common
