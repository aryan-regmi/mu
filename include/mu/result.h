#ifndef MU_RESULT_H
#define MU_RESULT_H

#include "mu/panic.h" // MU_PANIC
#include <utility>    // move

namespace mu {

/// Represents an error value.
template <typename E> struct Error {
  E err;
};

template <typename T> struct Ok {
public:
  Ok()                           = default;
  ~Ok()                          = default;
  Ok& operator=(Ok&& other)      = default;
  Ok& operator=(const Ok& other) = delete;

  explicit Ok(const T& val) : val(val) {}

  explicit Ok(T&& val) : val(std::move(val)) {}

  explicit Ok(Ok&& other) : val(std::move(other.val)) {}

private:
  T val;
  template <typename U, typename E> friend struct Result;
};
template <> struct Ok<void> {};

template <typename E> struct Err {
public:
  Err()                            = default;
  ~Err()                           = default;
  Err& operator=(Err&& other)      = default;
  Err& operator=(const Err& other) = delete;

  explicit Err(const E& val) : val(val) {}

  explicit Err(E&& val) : val(std::move(val)) {}

  explicit Err(Err&& other) : val(std::move(other.val)) {}

private:
  Error<E> val;
};

/// Represents either a valid `T` or an `Error<E>`.
template <typename T, typename E> class Result {
public:
  Result() noexcept  = default;
  ~Result() noexcept = default;

  Result(Ok<T> ok) noexcept : val(ok) {}

  Result(Err<E> err) noexcept : val(err) {}

  // Result(Ok<T>&& ok) noexcept : val(std::move(ok)) {}
  //
  // Result(Err<E>&& err) noexcept : val(std::move(err)) {}

  auto unwrap() noexcept -> T {
    if (this->val.index() == 0) {
      return std::get<T>(this->val);
    }

    MU_PANIC("Called `unwrap` on an error");
  }

  auto unwrapErr() noexcept -> T {
    if (this->val.index() == 1) {
      return std::get<Error<E>>(this->val);
    }

    MU_PANIC("Called `unwrapErr` on a non-error value");
  }

  auto isOk() noexcept -> bool { return this->val.index() == 0; }

  auto isErr() noexcept -> bool { return this->val.index() == 1; }

  // TODO: Add more functionality

private:
  std::variant<Ok<T>, Err<E>> val;
};

} // namespace mu

#endif // !MU_RESULT_H
