#ifndef MU_RESULT_H
#define MU_RESULT_H

#include "mu/cloneable.h" // Cloneable, Copyable
#include "mu/optional.h"
#include "mu/primitives.h"
#include <algorithm>
#include <exception>
#include <type_traits>
#include <utility>
#include <variant>

namespace mu {

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

/// Represents a success value in `Result<T, E>`.
template <typename T> struct Ok : Clone<Ok<T>> {
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

  T val;

private:
  auto cloneImpl() const -> Ok<T>
    requires(Cloneable<T>)
  {
    return Ok<T>(this->val.clone());
  }
};

/// Represents a success value in `Result<void, E>`.
template <> struct Ok<void> {
  Ok()                                    = default;
  ~Ok() noexcept                          = default;
  Ok(Ok&& other) noexcept                 = default;
  Ok(const Ok& other) noexcept            = default;
  Ok& operator=(const Ok& other) noexcept = default;
  Ok& operator=(Ok&& other) noexcept      = default;
};

/// Represents an error value in `Result<T, E>`.
template <typename E> struct Err : Clone<Err<E>> {
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

  E err;

private:
  auto cloneImpl() const -> Err<E>
    requires(Cloneable<E>)
  {
    return Err<E>(this->err.clone());
  }
};

// TODO: Add `IntoIter` mixin (and create an iterator!)
//
/// Represents either success (`Ok<T>`) or failure (`Err<E>`).
template <typename T, typename E> class Result : Clone<Result<T, E>> {
public:
  // TODO: Add copy constructors if T and E are copyable

  Result()                         = delete;
  Result(const Result&)            = delete;
  Result& operator=(const Result&) = delete;

  /// Creates an `Ok<T>` in-place.
  template <typename... Args>
  static auto create(Args... args) noexcept -> Result<T, E> {
    return Result(Ok<T>::create(std::forward(args)...));
  }

  /// Creates an `Err<E>` in-place.
  template <typename... Args>
  static auto createErr(Args... args) noexcept -> Result<T, E> {
    return Result(Err<E>::create(std::forward(args)...));
  }

  /// Move constructs an `Ok<T>` value.
  Result(Ok<T>&& ok_val) noexcept : val{std::move(ok_val)} {}

  /// Move constructs an `Err<E>` value.
  Result(Err<E>&& err_val) noexcept : val{std::move(err_val)} {}

  /// Move constructs a `Result<T, E>` from `other`.
  Result(Result&& other) noexcept {
    if (other.isOk()) {
      this->val = std::move(std::get<Ok<T>>(other.val));
    } else {
      this->val = std::move(std::get<Err<E>>(other.val));
    }
  }

  /// Move assigns a `Result<T, E>` from `other`.
  auto operator=(Result&& other) noexcept -> Result& {
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

  /// Destroys the contained value.
  ~Result() noexcept
    requires(noexcept(~T()) && noexcept(~E()))
  {
    if (this->isOk()) {
      auto& val = std::get<Ok<T>>(this->val);
      val.~Ok();
    } else {
      auto& err = std::get<Err<E>>(this->val);
      err.~Err();
    }
  }

  /// Destroys the contained value.
  ~Result() {
    if (this->isOk()) {
      auto& val = std::get<Ok<T>>(this->val);
      val.~Ok();
    } else {
      auto& err = std::get<Err<E>>(this->val);
      err.~Err();
    }
  }

  /// Returns `true` if the value is `Ok`.
  constexpr explicit operator bool() const noexcept { return this->isOk(); }

  /// Returns `true` if the value is `Ok`.
  constexpr auto     isOk() const noexcept -> bool {
    return this->val.index() == 0;
  }

  /// Returns `true` if the value is `Err`.
  constexpr auto isErr() const noexcept -> bool {
    return this->val.index() == 1;
  }

  /// Returns the contained `Ok` value.
  ///
  /// ## Note
  /// This will throw an `ResultUnwrapErrException` if the value is `Err`.
  auto unwrap() const -> const T& {
    if (this->isOk()) {
      auto& val = std::get<Ok<T>>(this->val);
      return val.val;
    }
    throw ResultUnwrapErrException();
  }

  /// Returns the contained `Ok` value.
  ///
  /// ## Note
  /// This will throw an `ResultUnwrapErrException` if the value is `Err`.
  auto unwrap() -> T& {
    if (this->isOk()) {
      auto& val = std::get<Ok<T>>(this->val);
      return val.val;
    }
    throw ResultUnwrapErrException();
  }

  /// Returns the contained `Err` value.
  ///
  /// ## Note
  /// This will throw an `ResultUnwrapOkException` if the value is `Ok`.
  auto unwrapErr() const -> const E& {
    if (this->isErr()) {
      auto& val = std::get<Err<E>>(this->val);
      return val.err;
    }
    throw ResultUnwrapOkException();
  }

  /// Returns the contained `Err` value.
  ///
  /// ## Note
  /// This will throw an `ResultUnwrapOkException` if the value is `Ok`.
  auto unwrapErr() -> E& {
    if (this->isErr()) {
      auto& val = std::get<Err<E>>(this->val);
      return val.err;
    }
    throw ResultUnwrapOkException();
  }

  /// Returns the contained `Ok` value or the provided default.
  auto unwrapOr(T&& default_val) const noexcept -> const T& {
    if (this->isOk()) {
      const auto& val = std::get<Ok<T>>(this->val);
      return val.val;
    }
    return default_val;
  }

  /// Returns the contained `Ok` value or the provided default.
  auto unwrapOr(T&& default_val) noexcept -> T& {
    if (this->isOk()) {
      auto& val = std::get<Ok<T>>(this->val);
      return val.val;
    }
    return default_val;
  }

  /// Returns the contained `Ok` value or calculates it from the provided
  /// function.
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

  /// Returns the contained `Ok` value or calculates it from the provided
  /// function.
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

  /// Maps a `Result<T, E>` to `Result<U, E>` by applying the function to the
  /// contained `Ok` value, leaving an `Err` value untouched.
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
    if constexpr (Copyable<E>) {
      return Result<U, E>(Err<E>{val});
    } else if constexpr (Cloneable<E>) {
      return Result<U, E>(val.clone());
    }
  }

  /// Maps a `Result<T, E>` to `Result<U, E>` by applying the function to the
  /// contained `Ok` value, leaving an `Err` value untouched.
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
    if constexpr (Copyable<E>) {
      return Result<U, E>(Err<E>{val});
    } else if constexpr (Cloneable<E>) {
      return Result<U, E>(val.clone());
    }
  }

  /// Maps a `Result<T, E>` to `Result<T, E2>` by applying the function to the
  /// contained `Err` value, leaving an `Ok` value untouched.
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
    if constexpr (Copyable<T>) {
      return Result<T, E2>(Ok<T>{val});
    } else if constexpr (Cloneable<T>) {
      return Result<T, E2>(val.clone());
    }
  }

  /// Maps a `Result<T, E>` to `Result<T, E2>` by applying the function to the
  /// contained `Err` value, leaving an `Ok` value untouched.
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
    if constexpr (Copyable<T>) {
      return Result<T, E2>(Ok<T>{val});
    } else if constexpr (Cloneable<T>) {
      return Result<T, E2>(val.clone());
    }
  }

  /// Creates an `Ok<T>` in-place, and destroys the contained value.
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

  /// Creates an `Ok<T>` in-place, and destroys the contained value.
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

  /// Creates an `Err<E>` in-place, and destroys the contained value.
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

  /// Creates an `Err<E>` in-place, and destroys the contained value.
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

  /// Converts a `Result<T, E>` to `Optional<T>`.
  ///
  /// Discards the value if it is `Err`.
  auto ok() -> Optional<T> {
    if (this->isOk()) {
      auto& val = std::get<Ok<T>>(this->val);
      return Optional<T>(std::move(val));
    }
    return Optional<T>();
  }

  /// Converts a `Result<T, E>` to `Optional<E>`.
  ///
  /// Discards the value if it is `Ok`.
  auto err() -> Optional<E> {
    if (this->isErr()) {
      auto& val = std::get<Err<E>>(this->val);
      return Optional<E>(std::move(val));
    }
    return Optional<E>();
  }

private:
  std::variant<Ok<T>, Err<E>> val;

  /// Clones the contained value.
  auto                        cloneImpl() const -> Result<T, E> {
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
};

template <typename E> struct Result<void, E> : Clone<Result<void, E>> {
public:
  // TODO: Add copy constructors if T and E are copyable

  Result()                         = delete;
  Result(const Result&)            = delete;
  Result& operator=(const Result&) = delete;

  /// Creates an `Err<E>` in-place.
  template <typename... Args>
  static auto createErr(Args... args) noexcept -> Result<void, E> {
    return Result(Err<E>::create(std::forward(args)...));
  }

  /// Move constructs an `Ok<void>` value.
  Result(Ok<void>&& /*ok_val*/) noexcept : val{Ok<void>()} {}

  /// Move constructs an `Err<E>` value.
  Result(Err<E>&& err_val) noexcept : val{std::move(err_val)} {}

  /// Move constructs a `Result<void, E>` from `other`.
  Result(Result&& other) noexcept {
    if (other.isOk()) {
      this->val = Ok<void>();
    } else {
      this->val = std::move(std::get<Err<E>>(other.val));
    }
  }

  /// Move assigns a `Result<void, E>` from `other`.
  Result& operator=(Result&& other) noexcept {
    if (*this == other) {
      return *this;
    }

    // Destory current
    if (this->isErr()) {
      auto& val = std::move(std::get<Err<E>>(other.val));
      val.~Err();
    }

    if (other.isOk()) {
      this->val = Ok<void>();
    } else {
      this->val = std::move(std::get<Err<E>>(other.val));
    }
    return *this;
  }

  /// Returns `true` if the value is `Ok`.
  constexpr explicit operator bool() const noexcept { return this->isOk(); }

  /// Returns `true` if the value is `Ok`.
  constexpr auto     isOk() const noexcept -> bool {
    return this->val.index() == 0;
  }

  /// Returns `true` if the value is `Err`.
  constexpr auto isErr() const noexcept -> bool {
    return this->val.index() == 1;
  }

  /// Returns the contained `Err` value.
  ///
  /// ## Note
  /// This will throw an `ResultUnwrapOkException` if the value is `Ok`.
  auto unwrapErr() const -> const E& {
    if (this->isErr()) {
      auto& val = std::get<Err<E>>(this->val);
      return val.err;
    }
    throw ResultUnwrapOkException();
  }

  /// Returns the contained `Err` value.
  ///
  /// ## Note
  /// This will throw an `ResultUnwrapOkException` if the value is `Ok`.
  auto unwrapErr() -> E& {
    if (this->isErr()) {
      auto& val = std::get<Err<E>>(this->val);
      return val.err;
    }
    throw ResultUnwrapOkException();
  }

  /// Maps a `Result<void, E>` to `Result<U, E>` by applying the function to the
  /// contained `Ok` value, leaving an `Err` value untouched.
  template <typename U, typename F>
  auto map(F&& func) const -> Result<U, E>
    requires requires(F&& func, U ret) {
      requires std::invocable<F>;
      { func() } -> std::same_as<U>;
    }
  {
    // Call mapping func if `Ok`
    if (this->isOk()) {
      return Result<U, E>(Ok<U>{func()});
    }

    // Return contained `Err` otherwise
    const auto& val = std::get<Err<E>>(this->val);
    if constexpr (Cloneable<E>) {
      // Clone error if it is cloneable
      return Result<U, E>(val.clone());
    } else if constexpr (Copyable<E>) {
      // Copy the error if copyable
      return Result<U, E>(Err<E>{val});
    }
  }

  /// Maps a `Result<void, E>` to `Result<U, E>` by applying the function to the
  /// contained `Ok` value, leaving an `Err` value untouched.
  template <typename U, typename F>
  auto map(F&& func) -> Result<U, E>
    requires requires(F&& func, U ret) {
      requires std::invocable<F>;
      { func() } -> std::same_as<U>;
    }
  {
    // Call mapping func if `Ok`
    if (this->isOk()) {
      return Result<U, E>(Ok<U>{func()});
    }

    // Return contained `Err` otherwise
    auto& val = std::get<Err<E>>(this->val);
    if constexpr (Cloneable<E>) {
      // Clone error if it is cloneable
      return Result<U, E>(val.clone());
    } else if constexpr (Copyable<E>) {
      // Copy the error if copyable
      return Result<U, E>(Err<E>{val});
    }
  }

  /// Maps a `Result<void, E>` to `Result<void, E2>` by applying the function to
  /// the contained `Err` value, leaving an `Ok` value untouched.
  template <typename E2, typename F>
  auto mapErr(F&& func) const -> Result<void, E2>
    requires requires(F&& func, const E& type, E2 ret) {
      requires std::invocable<F, const E&>;
      { func(type) } -> std::same_as<E2>;
    }
  {
    // Call mapping func if `Err`
    if (this->isErr()) {
      const auto& val = std::get<Err<E>>(this->val);
      return Result<void, E2>(Err<E2>{func(val.err)});
    }

    // Return contained `Ok` otherwise
    return Result<void, E2>(Ok<void>());
  }

  /// Maps a `Result<void, E>` to `Result<void, E2>` by applying the function to
  /// the contained `Err` value, leaving an `Ok` value untouched.
  template <typename E2, typename F>
  auto mapErr(F&& func) -> Result<void, E2>
    requires requires(F&& func, E& type, E2 ret) {
      requires std::invocable<F, E&>;
      { func(type) } -> std::same_as<E2>;
    }
  {
    // Call mapping func if `Err`
    if (this->isErr()) {
      auto& val = std::get<Err<E>>(this->val);
      return Result<void, E2>(Err<E2>{func(val.err)});
    }

    // Return contained `Ok` otherwise
    return Result<void, E2>(Ok<void>());
  }

  /// Creates an `Err<E>` in-place, and destroys the contained value.
  template <typename... Args>
  auto emplaceErr(Args... args) noexcept -> void
    requires(noexcept(E{std::forward<Args>(args)...}) && noexcept(~E()))
  {
    // Destroy old value
    if (this->isErr()) {
      auto& val = std::get<Err<E>>(this->val);
      val.~Err();
    }

    // Construct new value
    this->val = Err<E>{E{std::forward<Args>(args)...}};
  }

  /// Creates an `Err<E>` in-place, and destroys the contained value.
  template <typename... Args> auto emplaceErr(Args... args) -> void {
    // Destroy old value
    if (this->isErr()) {
      auto& val = std::get<Err<E>>(this->val);
      val.~Err();
    }

    // Construct new value
    this->val = Err<E>{E{std::forward<Args>(args)...}};
  }

private:
  // TODO: Replace Ok<void> w/ monostate?
  std::variant<Ok<void>, Err<E>> val;

  /// Clones the contained value.
  auto                           cloneImpl() const -> Result<void, E> {
    if (this->isOk()) {
      return Result<void, E>(Ok<void>());
    }

    const auto& val = std::get<Err<E>>(this->val);
    if constexpr (Cloneable<E>) {
      return Result<void, E>(val.clone());
    } else if constexpr (Copyable<E>) {
      return Result<void, E>(Err<E>{val});
    }
  }
};

} // namespace mu

#endif // !MU_RESULT_H
