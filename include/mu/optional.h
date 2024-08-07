#ifndef MU_OPTIONAL_H
#define MU_OPTIONAL_H

#include "mu/cloneable.h" // Cloneable, Clone
#include "mu/common.h"    // OptionUnwrapException
#include <concepts>       // same_as
#include <utility>        // move
#include <variant>

namespace mu {

// TODO: Add tests

/// An optional type that represents either a value of type `T` or an empty
/// value.
template <typename T> class Optional : Clone<Optional<T>> {
public:
  Optional(const Optional&) noexcept
    requires(Copyable<T>)
  = default;
  Optional& operator=(const Optional&) noexcept
    requires(Copyable<T>)
  = default;

  /// Constructs an object of type `T` and wraps it in an `Optional`.
  template <typename... Args>
  static auto create(Args... args) noexcept -> Optional<T>
    requires(noexcept(T{std::forward<Args>(args)...}))
  {
    return Optional(T{std::forward<Args>(args)...});
  }

  /// Constructs an object of type `T` and wraps it in an `Optional`.
  template <typename... Args> static auto create(Args... args) -> Optional<T> {
    return Optional(T{std::forward<Args>(args)...});
  }

  /// Constructs an `Optional` value that contains nothing (is empty).
  explicit Optional() noexcept = default;

  /// Move constructs an optional value containing `val`.
  explicit Optional(T&& val) noexcept : val{std::move(val)} {}

  /// Creates an `Optional` by transferring ownership from `other` to `this` and
  /// destroys `other`.
  Optional(Optional&& other) noexcept {
    if (other.isValid()) {
      this->val = std::get<T>(std::move(other.val));
    } else {
      this->val = std::monostate();
    }
    other.val = std::monostate();
  }

  /// Move assignment operator.
  /// Transfers the object contained from `other` to `this`.
  auto operator=(Optional&& other) noexcept -> Optional&
    requires(noexcept(~T()))
  {
    // Destroy contained object
    if (this->isValid()) {
      auto& val = std::get<T>(this->val);
      val.~T();
    }

    // Move `other`'s object if it exists
    if (other.isValid()) {
      this->val = std::get<T>(std::move(other.val));
    } else {
      this->val = std::monostate();
    }
    other.val = std::monostate();

    return *this;
  }

  /// Move assignment operator.
  /// Transfers the object contained from `other` to `this`.
  auto operator=(Optional&& other) -> Optional& {
    // Destroy contained object
    if (this->isValid()) {
      auto& val = std::get<T>(this->val);
      val.~T();
    }

    // Move `other`'s object if it exists
    if (other.isValid()) {
      this->val = std::get<T>(std::move(other.val));
    } else {
      this->val = std::monostate();
    }
    other.val = std::monostate();

    return *this;
  }

  /// Destroys the contained object.
  ~Optional() noexcept
    requires(noexcept(~T()))
  {
    if (this->isValid()) {
      auto& val = std::get<T>(this->val);
      val.~T();
    }
  }

  /// Destroys the contained object.
  ~Optional() {
    if (this->isValid()) {
      auto& val = std::get<T>(this->val);
      val.~T();
    }
  }

  /// Checks if `this` is a valid optional.
  ///
  /// Returns `true` if `this.isValid() == true`.
  constexpr explicit operator bool() const noexcept { return this->isValid(); }

  /// Checks if `this` is a valid optional.
  constexpr auto     isValid() const noexcept -> bool {
    return this->val.index() == 1;
  }

  /// Get the value stored in the `Optional`.
  ///
  /// ## Note
  /// This will throw an `OptionUnwrapException` if there is no contained value.
  auto unwrap() const -> const T& {
    if (this->isValid()) {
      return std::get<T>(this->val);
    }
    throw common::OptionUnwrapException();
  }

  /// Get the value stored in the `Optional`.
  ///
  /// ## Note
  /// This will throw an `OptionUnwrapException` if there is no contained value.
  auto unwrap() -> T& {
    if (this->isValid()) {
      return std::get<T>(this->val);
    }
    throw common::OptionUnwrapException();
  }

  /// Get the contained value or return the provided default.
  auto unwrapOr(T&& default_val) const noexcept -> const T& {
    if (this->isValid()) {
      return std::get<T>(this->val);
    }
    return default_val;
  }

  /// Get the contained value or return the provided default.
  auto unwrapOr(T&& default_val) noexcept -> T& {
    if (this->isValid()) {
      return std::get<T>(this->val);
    }
    return default_val;
  }

  /// Get the contained value or calculate the default value by calling `func`.
  ///
  /// ## Note
  /// `F` must have the signature `func() -> T`.
  template <typename F>
  auto unwrapOrElse(F&& func) const noexcept -> const T&
    requires requires(F&& func, T type) {
      requires std::invocable<F>;
      { func() } -> std::same_as<T>;
    }
  {
    if (this->isValid()) {
      return std::get<T>(this->val);
    }
    return func();
  }

  /// Get the contained value or calculate the default value by calling `func`.
  ///
  /// ## Note
  /// `F` must have the signature `func() -> T`.
  template <typename F>
  auto unwrapOrElse(F&& func) noexcept -> T&
    requires requires(F&& func, T type) {
      requires std::invocable<F>;
      { func() } -> std::same_as<T>;
    }
  {
    if (this->isValid()) {
      return std::get<T>(this->val);
    }
    return func();
  }

  /// Maps an `Optional<T>` to an `Optional<U>` by calling `func` on the
  /// contained value.
  ///
  /// If there is no contained value, this returns an empty optional
  /// (`Optional<U>()`).
  ///
  /// ## Note
  /// `F` must have the signature `func(const T&) -> U`.
  template <typename U, typename F>
  auto map(F&& func) const -> Optional<U>
    requires requires(F&& func, const T& type, U ret) {
      requires std::invocable<F, const T&>;
      { func(type) } -> std::same_as<U>;
    }
  {
    if (this->isValid()) {
      return Optional<U>(func(std::get<T>(this->val)));
    }
    return Optional<U>();
  }

  /// Maps an `Optional<T>` to an `Optional<U>` by calling `func` on the
  /// contained value.
  ///
  /// If there is no contained value, this returns an empty optional
  /// (`Optional<U>()`).
  ///
  /// ## Note
  /// `F` must have the signature `func(T&) -> U`.
  template <typename U, typename F>
  auto map(F&& func) -> Optional<U>
    requires requires(F&& func, T& type, U ret) {
      requires std::invocable<F, T&>;
      { func(type) } -> std::same_as<U>;
    }
  {
    if (this->isValid()) {
      return Optional<U>(func(std::get<T>(this->val)));
    }
    return Optional<U>();
  }

  /// Replaces the current value by `val`.
  ///
  /// This will call the destructor of the contained value.
  auto replace(T&& val) noexcept -> void
    requires(noexcept(~T()))
  {
    if (this->isValid()) {
      auto& val = std::get<T>(this->val);
      val.~T();
    }
    this->val = std::move(val);
  }

  /// Replaces the current value by `val`.
  ///
  /// This will call the destructor of the contained value.
  auto replace(T&& val) -> void {
    if (this->isValid()) {
      auto& val = std::get<T>(this->val);
      val.~T();
    }
    this->val = std::move(val);
  }

  /// Replaces the current value by constructing a new value of `T` in-place.
  ///
  /// This will call the destructor of the contained value.
  template <typename... Args>
  auto emplace(Args... args) noexcept -> void
    requires(noexcept(T{std::forward<Args>(args)...}) && noexcept(~T()))
  {
    if (this->isValid()) {
      auto& val = std::get<T>(this->val);
      val.~T();
    }
    this->val = T{std::forward<Args>(args)...};
  }

  /// Replaces the current value by constructing a new value of `T` in-place.
  ///
  /// This will call the destructor of the contained value.
  template <typename... Args> auto emplace(Args... args) -> void {
    if (this->isValid()) {
      auto& val = std::get<T>(this->val);
      val.~T();
    }
    this->val = T{std::forward<Args>(args)...};
  }

  /// Takes the value out of the `Optional`, leaving an empty optional in its
  /// place.
  auto take() noexcept -> Optional<T> {
    if (this->isValid()) {
      Optional<T> ret{std::get<T>(std::move(this->val))};
      this->val = std::monostate();
      return ret;
    }
    return Optional();
  }

  /// Destroys any contained value, but leaves the `Optional` intact.
  auto reset() noexcept
    requires(noexcept(~T()))
  {
    if (this->isValid()) {
      auto& val = std::get<T>(this->val);
      val.~T();
      this->val = std::monostate();
    }
  }

  /// Destroys any contained value, but leaves the `Optional` intact.
  auto reset() {
    if (this->isValid()) {
      auto& val = std::get<T>(this->val);
      val.~T();
      this->val = std::monostate();
    }
  }

  /// Calls `func` on the contained value and returns the result.
  /// Returns an empty `Optional` if `this` is empty.
  ///
  /// ## Note
  /// `F` must have the signature `func(const T&) -> U`.
  template <typename U, typename F>
  auto andThen(F&& func) const -> Optional<U>
    requires requires(F&& func, const T& type, U ret) {
      requires std::invocable<F, const T&>;
      { func(type) } -> std::same_as<U>;
    }
  {
    if (this->isValid()) {
      return Optional<U>(func(std::get<T>(this->val)));
    }
    return Optional<U>();
  }

  /// Calls `func` on the contained value and returns the result.
  /// Returns an empty `Optional` if `this` is empty.
  ///
  /// ## Note
  /// `F` must have the signature `func(T&) -> U`.
  template <typename U, typename F>
  auto andThen(F&& func) -> Optional<U>
    requires requires(F&& func, T& type, U ret) {
      requires std::invocable<F, T&>;
      { func(type) } -> std::same_as<U>;
    }
  {
    if (this->isValid()) {
      return Optional<U>(func(std::get<T>(this->val)));
    }
    return Optional<U>();
  }

private:
  std::variant<std::monostate, T> val;

  /// Clones the contained value.
  auto                            cloneImpl() const noexcept -> Optional<T>
    requires(Cloneable<T>)
  {
    if (!this->isValid()) {
      return Optional();
    }

    if constexpr (Copyable<T>) {
      return *this;
    } else if constexpr (Cloneable<T>) {
      auto& val = std::get<T>(this->val);
      return Optional(val.clone());
    }
  }
};

} // namespace mu

#endif // !MU_OPTIONAL_H
