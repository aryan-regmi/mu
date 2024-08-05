#ifndef MU_RESULT_H
#define MU_RESULT_H

#include "mu/cloneable.h"
#include "mu/optional.h"
#include "mu/primitives.h"
#include <algorithm>
#include <exception>
#include <utility>
#include <variant>

namespace mu {

// TODO: Add doc comments

// TODO: Add tests

/// The exception thrown if `unwrap` is called on an `Err`.
struct ResultUnwrapErrException : std::exception {
  auto what() const throw() -> const_cstr override {
    return "ResultUnwrapErrException: Called `unwrap` on result containing an "
           "`Err`; use `isOk` to check if the result is an `Ok` type first";
  }
};

/// The exception thrown if `unwrapErr` is called on an `Ok`.
struct ResultUnwrapOkException : std::exception {
  auto what() const throw() -> const_cstr override {
    return "ResultUnwrapOkException: Called `unwrapErr` on result containing "
           "an "
           "`Ok`; use `isErr` to check if the result is an `Err` type first";
  }
};

template <typename T> struct Ok {
  Ok()                    = delete;
  ~Ok() noexcept          = default;
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

  auto clone() const -> Ok<T>
    requires(Cloneable<T>)
  {
    return Ok<T>(this->val.clone());
  }

  T val;
};

template <typename E> struct Err {
  Err()                     = delete;
  ~Err() noexcept           = default;
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

  auto clone() const -> Err<E>
    requires(Cloneable<E>)
  {
    return Err<E>(this->err.clone());
  }

  E err;
};

// TODO: Add `IntoIter` mixin (and create an iterator!)
//
// TODO: Specialize for Result<void, E>.
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

  Result(Ok<T>&& ok_val) noexcept : val{std::move(ok_val)} {}

  Result(Err<E>&& err_val) noexcept : val{std::move(err_val)} {}

  Result(Result&& other) noexcept {
    if (other.isOk()) {
      this->val = std::move(std::get<Ok<T>>(other.val));
    } else {
      this->val = std::move(std::get<Err<E>>(other.val));
    }
  }

  Result& operator=(Result&& other) noexcept {
    if (*this == other) {
      return *this;
    }

    // Destory current
    if (this->isOk()) {
      auto& val = std::move(std::get<Ok<T>>(other.val));
      val.~Ok();
    } else {
      auto& val = std::move(std::get<Err<E>>(other.val));
      val.~Err();
    }

    if (other.isOk()) {
      this->val = std::move(std::get<Ok<T>>(other.val));
    } else {
      this->val = std::move(std::get<Err<E>>(other.val));
    }
    return *this;
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
      auto& val = std::get<Ok<T>>(this->val);
      return val.val;
    }
    throw ResultUnwrapErrException();
  }

  auto unwrap() -> T& {
    if (this->isOk()) {
      auto& val = std::get<Ok<T>>(this->val);
      return val.val;
    }
    throw ResultUnwrapErrException();
  }

  auto unwrapErr() const -> const E& {
    if (this->isErr()) {
      auto& val = std::get<Err<E>>(this->val);
      return val.err;
    }
    throw ResultUnwrapOkException();
  }

  auto unwrapErr() -> E& {
    if (this->isErr()) {
      auto& val = std::get<Err<E>>(this->val);
      return val.err;
    }
    throw ResultUnwrapOkException();
  }

  auto unwrapOr(T&& default_val) const noexcept -> const T& {
    if (this->isOk()) {
      const auto& val = std::get<Ok<T>>(this->val);
      return val.val;
    }
    return default_val;
  }

  auto unwrapOr(T&& default_val) noexcept -> T& {
    if (this->isOk()) {
      auto& val = std::get<Ok<T>>(this->val);
      return val.val;
    }
    return default_val;
  }

  template <typename F>
  auto unwrapOrElse(F&& func) const noexcept -> const T&
    requires requires(F&& func, T type) {
      requires std::invocable<F>;
      { func() } -> std::same_as<T>;
    }
  {
    if (this->isOk()) {
      const auto& val = std::get<Ok<T>>(this->val);
      return val.val;
    }
    return func();
  }

  template <typename F>
  auto unwrapOrElse(F&& func) noexcept -> T&
    requires requires(F&& func, T type) {
      requires std::invocable<F>;
      { func() } -> std::same_as<T>;
    }
  {
    if (this->isOk()) {
      auto& val = std::get<Ok<T>>(this->val);
      return val.val;
    }
    return func();
  }

  template <typename U, typename F>
  auto map(F&& func) const -> Result<U, E>
    requires requires(F&& func, const T& type, U ret) {
      requires std::invocable<F, const T&>;
      { func(type) } -> std::same_as<U>;
    }
  {
    // Call mapping func if `Ok`
    if (this->isOk()) {
      const auto& val = std::get<Ok<T>>(this->val);
      return Result<U, E>(Ok<U>{func(val.val)});
    }

    // Return contained `Err` otherwise
    const auto& val = std::get<Err<E>>(this->val);
    if constexpr (Cloneable<E>) {
      // Clone error if it is cloneable
      return Result<U, E>(val.clone());
    } else if constexpr (Copyable<T>) {
      // Copy the error if copyable
      return Result<U, E>(Err<E>{val});
    }
  }

  template <typename U, typename F>
  auto map(F&& func) -> Result<U, E>
    requires requires(F&& func, T& type, U ret) {
      requires std::invocable<F, T&>;
      { func(type) } -> std::same_as<U>;
    }
  {
    // Call mapping func if `Ok`
    if (this->isOk()) {
      auto& val = std::get<Ok<T>>(this->val);
      return Result<U, E>(Ok<U>{func(val.val)});
    }

    // Return contained `Err` otherwise
    auto& val = std::get<Err<E>>(this->val);
    if constexpr (Cloneable<E>) {
      // Clone error if it is cloneable
      return Result<U, E>(val.clone());
    } else if constexpr (Copyable<T>) {
      // Copy the error if copyable
      return Result<U, E>(Err<E>{val});
    }
  }

  template <typename E2, typename F>
  auto mapErr(F&& func) const -> Result<T, E2>
    requires requires(F&& func, const E& type, E2 ret) {
      requires std::invocable<F, const E&>;
      { func(type) } -> std::same_as<E2>;
    }
  {
    // Call mapping func if `Err`
    if (this->isErr()) {
      const auto& val = std::get<Err<E>>(this->val);
      return Result<T, E2>(Err<E2>{func(val.err)});
    }

    // Return contained `Ok` otherwise
    const auto& val = std::get<Ok<T>>(this->val);
    if constexpr (Cloneable<T>) {
      // Clone value if it is cloneable
      return Result<T, E2>(val.clone());
    } else if constexpr (Copyable<T>) {
      // Copy the value if copyable
      return Result<T, E2>(Ok<T>{val});
    }
  }

  template <typename E2, typename F>
  auto mapErr(F&& func) -> Result<T, E2>
    requires requires(F&& func, E& type, E2 ret) {
      requires std::invocable<F, E&>;
      { func(type) } -> std::same_as<E2>;
    }
  {
    // Call mapping func if `Err`
    if (this->isErr()) {
      auto& val = std::get<Err<E>>(this->val);
      return Result<T, E2>(Err<E2>{func(val.err)});
    }

    // Return contained `Ok` otherwise
    auto& val = std::get<Ok<T>>(this->val);
    if constexpr (Cloneable<T>) {
      // Clone value if it is cloneable
      return Result<T, E2>(val.clone());
    } else if constexpr (Copyable<T>) {
      // Copy the value if copyable
      return Result<T, E2>(Ok<T>{val});
    }
  }

  template <typename... Args>
  auto emplace(Args... args) noexcept -> void
    requires(noexcept(T{std::forward<Args>(args)...}) && noexcept(~T()) &&
             noexcept(~E()))
  {
    // Destroy old value
    if (this->isOk()) {
      auto& val = std::get<Ok<T>>(this->val);
      val.~Ok();
    } else {
      auto& val = std::get<Err<E>>(this->val);
      val.~Err();
    }

    // Construct new value
    this->val = Ok<T>{T{std::forward<Args>(args)...}};
  }

  template <typename... Args> auto emplace(Args... args) -> void {
    // Destroy old value
    if (this->isOk()) {
      auto& val = std::get<Ok<T>>(this->val);
      val.~Ok();
    } else {
      auto& val = std::get<Err<E>>(this->val);
      val.~Err();
    }

    // Construct new value
    this->val = Ok<T>{T{std::forward<Args>(args)...}};
  }

  template <typename... Args>
  auto emplaceErr(Args... args) noexcept -> void
    requires(noexcept(E{std::forward<Args>(args)...}) && noexcept(~T()) &&
             noexcept(~E()))
  {
    // Destroy old value
    if (this->isOk()) {
      auto& val = std::get<Ok<T>>(this->val);
      val.~Ok();
    } else {
      auto& val = std::get<Err<E>>(this->val);
      val.~Err();
    }

    // Construct new value
    this->val = Err<E>{E{std::forward<Args>(args)...}};
  }

  template <typename... Args> auto emplaceErr(Args... args) -> void {
    // Destroy old value
    if (this->isOk()) {
      auto& val = std::get<Ok<T>>(this->val);
      val.~Ok();
    } else {
      auto& val = std::get<Err<E>>(this->val);
      val.~Err();
    }

    // Construct new value
    this->val = Err<E>{E{std::forward<Args>(args)...}};
  }

  auto ok() -> Optional<T> {
    if (this->isOk()) {
      auto& val = std::get<Ok<T>>(this->val);
      return Optional<T>(std::move(val));
    }
    return Optional<T>();
  }

  auto err() -> Optional<E> {
    if (this->isErr()) {
      auto& val = std::get<Err<E>>(this->val);
      return Optional<E>(std::move(val));
    }
    return Optional<E>();
  }

  auto clone() const -> Result<T, E> {
    if (this->isOk()) {
      const auto& val = std::get<Ok<T>>(this->val);
      if constexpr (Cloneable<T>) {
        return Result<T, E>(val.clone());
      } else if constexpr (Copyable<T>) {
        return Result<T, E>(Ok<T>{val});
      }
    } else {
      const auto& val = std::get<Err<E>>(this->val);
      if constexpr (Cloneable<E>) {
        return Result<T, E>(val.clone());
      } else if constexpr (Copyable<E>) {
        return Result<T, E>(Err<E>{val});
      }
    }
  }

private:
  std::variant<Ok<T>, Err<E>> val;
};

} // namespace mu

#endif // !MU_RESULT_H
