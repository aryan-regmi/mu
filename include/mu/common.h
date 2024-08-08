#ifndef MU_COMMON_H
#define MU_COMMON_H

#include "mu/primitives.h" // usize, u8
#include <exception>       // exception

// TODO: Make **all** constructor calls consistent (use brace-initialization)
//
// TODO: Put all exception impls in cpp file!
//
// TODO: Make sure `rule of 5` is taken into account for everything

namespace mu::common {

// TODO: Put all common exceptions here!

/// The exception thrown if an allocation failed.
struct OutOfMemoryException : public std::exception {
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
struct OptionUnwrapException : public std::exception {
  /// Explains the error.
  auto what() const throw() -> const_cstr override;
};

/// The exception thrown if `unwrap` is called on an `Err`.
struct ResultUnwrapErrException : public std::exception {
  /// Explains the error.
  auto what() const throw() -> const_cstr override;
};

/// The exception thrown if `unwrapErr` is called on an `Ok`.
struct ResultUnwrapOkException : public std::exception {
  /// Explains the error.
  auto what() const throw() -> const_cstr override;
};

/// Index out of bounds exception.
struct IndexOutOfBounds : public std::exception {
  explicit IndexOutOfBounds(usize idx, usize len) : idx{idx}, len{len} {}

  /// Explains the error.
  auto  what() const throw() -> const_cstr override;

  usize idx;
  usize len;
};

} // namespace mu::common

#endif // !MU_COMMON_H
