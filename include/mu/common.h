#ifndef MU_COMMON_H
#define MU_COMMON_H

#include "mu/primitives.h" // usize, u8
#include <exception>       // exception

// TODO: Make **all** constructor calls consistent (use brace-initialization)
//
// TODO: Put all exception impls in cpp file!

namespace mu::common {

// TODO: Put all common exceptions here!

/// The exception thrown if an allocation failed.
class OutOfMemoryException : std::exception {
  explicit OutOfMemoryException(usize allocation_size)
      : allocation_size{allocation_size} {}

  /// Explains the error.
  auto what() const throw() -> const_cstr override;

private:
  static const usize BUFSIZE = 256;
  u8                 buf[BUFSIZE];
  usize              allocation_size;
};

/// The exception thrown if `unwrap` is called on an empty `Optional`.
struct OptionUnwrapException : std::exception {
  /// Explains the error.
  auto what() const throw() -> const_cstr override;
};

/// The exception thrown if `unwrap` is called on an `Err`.
struct ResultUnwrapErrException : std::exception {
  /// Explains the error.
  auto what() const throw() -> const_cstr override;
};

/// The exception thrown if `unwrapErr` is called on an `Ok`.
struct ResultUnwrapOkException : std::exception {
  /// Explains the error.
  auto what() const throw() -> const_cstr override;
};

/// Index out of bounds exception.
struct IndexOutOfBounds : std::exception {
  explicit IndexOutOfBounds(usize idx, usize len) : idx{idx}, len{len} {}

  /// Explains the error.
  auto  what() const throw() -> const_cstr override;

  usize idx;
  usize len;
};

} // namespace mu::common

#endif // !MU_COMMON_H
