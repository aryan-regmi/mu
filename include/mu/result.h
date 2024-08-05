#ifndef MU_RESULT_H
#define MU_RESULT_H

#include "mu/cloneable.h"
#include "mu/primitives.h"
#include <algorithm>
#include <exception>
#include <type_traits>
#include <utility>
#include <variant>

namespace mu {

/// The exception thrown if `unwrap` is called on an empty `Optional`.
struct ResultUnwrapErrException : std::exception {
  auto what() const throw() -> const_cstr override {
    return "ResultUnwrapErrException: Called `unwrap` on result containing an "
           "`Err`; use `isOk` to check if the result is an `Ok` type first";
  }
};

struct ResultUnwrapOkException : std::exception {
  auto what() const throw() -> const_cstr override {
    return "ResultUnwrapOkException: Called `unwrapErr` on result containing "
           "an "
           "`Ok`; use `isErr` to check if the result is an `Err` type first";
  }
};

template <typename T> struct Ok {
  Ok()                    = delete;
  Ok(Ok&& other) noexcept = default;
  Ok(const Ok& other) noexcept
    requires(Copyable<T>)
  = default;
  Ok& operator=(const Ok& other) noexcept
    requires(Copyable<T>)
  = default;
  Ok& operator=(Ok&& other) noexcept = default;

  template <typename... Args>
  static auto create(Args... args) noexcept -> Ok<T>
    requires(noexcept(T{std::forward<Args>(args)...}))
  {
    return Ok(T{std::forward<Args>(args)...});
  }

  template <typename... Args> static auto create(Args... args) -> Ok<T> {
    return Ok(T{std::forward<Args>(args)...});
  }

  explicit Ok(T&& val) noexcept : val{std::move(val)} {}

  ~Ok() noexcept
    requires(std::is_trivially_destructible_v<T> && noexcept(~T()))
  = default;

  ~Ok()
    requires(std::is_trivially_destructible_v<T>)
  = default;

  ~Ok() noexcept
    requires(noexcept(~T()))
  {
    this->val.~T();
  }

  ~Ok() { this->val.~T(); }

  T    val;

  auto clone() const -> Ok<T>
    requires(Cloneable<T>)
  {
    return Ok<T>(this->val.clone());
  }
};

template <typename E> struct Err {
  Err()                     = delete;
  Err(Err&& other) noexcept = default;
  Err(const Err& other) noexcept
    requires(Copyable<E>)
  = default;
  Err& operator=(const Err& other) noexcept
    requires(Copyable<E>)
  = default;
  Err& operator=(Err&& other) noexcept = default;

  template <typename... Args>
  static auto create(Args... args) noexcept -> Err<E>
    requires(noexcept(E{std::forward<Args>(args)...}))
  {
    return Err(E{std::forward<Args>(args)...});
  }

  template <typename... Args> static auto create(Args... args) -> Err<E> {
    return Err(E{std::forward<Args>(args)...});
  }

  explicit Err(E&& val) noexcept : err{std::move(val)} {}

  ~Err() noexcept
    requires(std::is_trivially_destructible_v<E> && noexcept(~E()))
  = default;

  ~Err()
    requires(std::is_trivially_destructible_v<E>)
  = default;

  ~Err() noexcept
    requires(noexcept(~E()))
  {
    this->val.~E();
  }

  ~Err() { this->val.~E(); }

  E    err;

  auto clone() const -> Err<E>
    requires(Cloneable<E>)
  {
    return Err<E>(this->err.clone());
  }
};

template <typename T, typename E> class Result {
public:
  Result()                         = delete;
  Result(const Result&)            = delete;
  Result& operator=(const Result&) = delete;

  template <typename... Args>
  static auto create(Args... args) noexcept -> Result<T, E> {
    return Result(Ok<T>::create(std::forward(args)...));
  }

  template <typename... Args>
  static auto createErr(Args... args) noexcept -> Result<T, E> {
    return Result(Err<E>::create(std::forward(args)...));
  }

  Result(Ok<T>&& ok_val) noexcept : val{ok_val} {}

  Result(Err<E>&& err_val) noexcept : val{err_val} {}

  Result(Result&& other) noexcept {
    if (other.isOk()) {
      this->val = std::move(std::get<Ok<T>>(other.val));
    } else {
      this->val = std::move(std::get<Err<E>>(other.val));
    }
  }

  constexpr explicit operator bool() const noexcept { return this->isOk(); }

  constexpr auto     isOk() const noexcept -> bool {
    return this->val.index() == 0;
  }

  constexpr auto isErr() const noexcept -> bool {
    return this->val.index() == 1;
  }

  auto unwrap() const -> const T& {
    if (this->isOk()) {
      Ok<T> val = std::get<Ok<T>>(this->val);
      return val.val;
    }
    throw ResultUnwrapErrException();
  }

  auto unwrap() -> T& {
    if (this->isOk()) {
      Ok<T> val = std::get<Ok<T>>(this->val);
      return val.val;
    }
    throw ResultUnwrapErrException();
  }

  auto unwrapErr() const -> const E& {
    if (this->isErr()) {
      auto val = std::get<Err<E>>(this->val);
      return val.err;
    }
    throw ResultUnwrapOkException();
  }

  auto unwrapErr() -> E& {
    if (this->isErr()) {
      Err<E> val = std::get<Err<E>>(this->val);
      return val.err;
    }
    throw ResultUnwrapOkException();
  }

  template <typename U, typename F>
  auto map(F&& func) const -> Result<U, E>
    requires requires(F&& func, const T& type, U ret) {
      std::invocable<F, const T&>;
      { func(type) } -> std::same_as<U>;
    }
  {
    // Call mapping func if `Ok`
    if (this->isOk()) {
      const Ok<T> val = std::get<Ok<T>>(this->val);
      return Result<U, E>(Ok<U>{func(val.val)});
    }

    // Return contained `Err` otherwise
    const Err<E> val = std::get<Err<E>>(this->val);
    if constexpr (Cloneable<E>) {
      // Clone error if it is cloneable
      return Result<U, E>(val.clone());
    } else {
      // Copy the error if trivially copyable
      return Result<U, E>(Err<E>{val});
    }
  }

  template <typename U, typename F>
  auto map(F&& func) -> Result<U, E>
    requires requires(F&& func, T& type, U ret) {
      std::invocable<F, T&>;
      { func(type) } -> std::same_as<U>;
    }
  {
    // Call mapping func if `Ok`
    if (this->isOk()) {
      Ok<T> val = std::get<Ok<T>>(this->val);
      return Result<U, E>(Ok<U>{func(val.val)});
    }

    // Return contained `Err` otherwise
    Err<E> val = std::get<Err<E>>(this->val);
    if constexpr (Cloneable<E>) {
      // Clone error if it is cloneable
      return Result<U, E>(val.clone());
    } else {
      // Copy the error if trivially copyable
      return Result<U, E>(Err<E>{val});
    }
  }

  template <typename E2, typename F>
  auto mapErr(F&& func) const -> Result<T, E2>
    requires requires(F&& func, const E& type, E2 ret) {
      std::invocable<F, const E&>;
      { func(type) } -> std::same_as<E2>;
    }
  {
    // Call mapping func if `Err`
    if (this->isErr()) {
      const Err<E> val = std::get<Err<E>>(this->val);
      return Result<T, E2>(Err<E2>{func(val.err)});
    }

    // Return contained `Ok` otherwise
    const Ok<T> val = std::get<Ok<T>>(this->val);
    if constexpr (Cloneable<T>) {
      // Clone value if it is cloneable
      return Result<T, E2>(val.clone());
    } else {
      // Copy the value if trivially copyable
      return Result<T, E2>(Ok<T>{val});
    }
  }

private:
  std::variant<Ok<T>, Err<E>> val;
};

} // namespace mu

#endif // !MU_RESULT_H
