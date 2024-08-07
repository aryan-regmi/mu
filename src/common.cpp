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

} // namespace mu::common
