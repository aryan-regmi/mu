#ifndef MU_OPTIONAL_H
#define MU_OPTIONAL_H

#include "mu/cloneable.h"  // Cloneable
#include "mu/primitives.h" // const_cstr
#include <concepts>        // same_as
#include <exception>       // noexcept, exception
#include <type_traits>     // is_trivially_destructible_v
#include <utility>         // move

namespace mu {

// TODO: Add tests

/// The exception thrown if `unwrap` is called on an empty `Optional`.
struct OptionUnwrapException : std::exception {
  auto what() const throw() -> const_cstr override {
    return "OptionUnwrapException: Called `unwrap` on an empty optional; use "
           "`isValid` to check if the optional value exists first";
  }
};

// TODO: Add `IntoIter` mixin (and create an iterator!)
//
// TODO: Specialize for Optional<void>.
//
/// An optional type that represents either a value of type `T` or an empty
/// value.
template <typename T> class Optional {
public:
  Optional(const Optional&)            = delete;
  Optional& operator=(const Optional&) = delete;

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
  explicit Optional() noexcept : none{false}, valid{false} {}

  /// Move constructs an optional value containing `val`.
  explicit Optional(T&& val) noexcept : some{std::move(val)}, valid{true} {}

  /// Creates an `Optional` by transferring ownership from `other` to `this` and
  /// destroys `other`.
  Optional(Optional&& other) noexcept {
    this->valid = other.valid;
    if (other.valid) {
      this->some = std::move(other.some);
    } else {
      this->dummy = 0;
    }
    other.valid = false;
  }

  /// Move assignment operator.
  /// Transfers the object contained from `other` to `this`.
  auto operator=(Optional&& other) noexcept -> Optional&
    requires(noexcept(~T()))
  {
    // Destroy contained object
    if (this->valid) {
      this->some.~T();
    }

    /// Move `other`'s object if it exists
    this->valid = other.valid;
    if (other.valid) {
      this->some = std::move(other.val);
    } else {
      this->dummy = false;
    }
    other.valid = false;
  }

  /// Move assignment operator.
  /// Transfers the object contained from `other` to `this`.
  auto operator=(Optional&& other) -> Optional& {
    // Destroy contained object
    if (this->valid) {
      this->some.~T();
    }

    /// Move `other`'s object if it exists
    this->valid = other.valid;
    if (other.valid) {
      this->some = std::move(other.val);
    } else {
      this->dummy = false;
    }
    other.valid = false;
  }

  /// Destroys the contained object.
  ~Optional() noexcept
    requires(std::is_trivially_destructible_v<T> && noexcept(~T()))
  = default;

  /// Destroys the contained object.
  ~Optional()
    requires(std::is_trivially_destructible_v<T>)
  = default;

  /// Destroys the contained object.
  ~Optional() noexcept
    requires(noexcept(~T()))
  {
    if (valid) {
      this->some.~T();
    }
  }

  /// Destroys the contained object.
  ~Optional() {
    if (valid) {
      this->some.~T();
    }
  }

  /// Checks if `this` is a valid optional.
  ///
  /// Returns `true` if `this.valid == true`.
  constexpr explicit operator bool() const noexcept { return this->valid; }

  /// Checks if `this` is a valid optional.
  ///
  /// Returns `true` if `this.valid == true`.
  constexpr auto     isValid() const noexcept -> bool { return this->valid; }

  /// Get the value stored in the `Optional`.
  ///
  /// ## Note
  /// This will throw an `OptionUnwrapException` if there is no contained value.
  auto               unwrap() const -> const T& {
    if (this->valid) {
      return this->some;
    }
    throw OptionUnwrapException();
  }

  /// Get the value stored in the `Optional`.
  ///
  /// ## Note
  /// This will throw an `OptionUnwrapException` if there is no contained value.
  auto unwrap() -> T& {
    if (this->valid) {
      return this->some;
    }
    throw OptionUnwrapException();
  }

  /// Get the contained value or return the provided default.
  auto unwrapOr(T&& default_val) const noexcept -> const T& {
    if (this->valid) {
      return this->some;
    }
    return default_val;
  }

  /// Get the contained value or return the provided default.
  auto unwrapOr(T&& default_val) noexcept -> T& {
    if (this->valid) {
      return this->some;
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
      std::invocable<F>;
      { func() } -> std::same_as<T>;
    }
  {
    if (this->valid) {
      return this->some;
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
      std::invocable<F>;
      { func() } -> std::same_as<T>;
    }
  {
    if (this->valid) {
      return this->some;
    }
    return func();
  }

  // TODO: Add `except` method

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
      std::invocable<F, const T&>;
      { func(type) } -> std::same_as<U>;
    }
  {
    if (this->valid) {
      return Optional<U>(func(this->some));
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
      std::invocable<F, T&>;
      { func(type) } -> std::same_as<U>;
    }
  {
    if (this->valid) {
      return Optional<U>(func(this->some));
    }
    return Optional<U>();
  }

  /// Replaces the current value by `val`.
  ///
  /// This will call the destructor of the contained value.
  auto replace(T&& val) noexcept -> void
    requires(noexcept(~T()))
  {
    if (this->valid) {
      this->some.~T();
    }
    this->some = std::move(val);
  }

  /// Replaces the current value by `val`.
  ///
  /// This will call the destructor of the contained value.
  auto replace(T&& val) -> void {
    if (this->valid) {
      this->some.~T();
    }
    this->some = std::move(val);
  }

  /// Replaces the current value by constructing a new value of `T` in-place.
  ///
  /// This will call the destructor of the contained value.
  template <typename... Args>
  auto emplace(Args... args) noexcept -> void
    requires(noexcept(T{std::forward<Args>(args)...}) && noexcept(~T()))
  {
    if (this->valid) {
      this->some.~T();
    }
    this->some = T{std::forward<Args>(args)...};
  }

  /// Replaces the current value by constructing a new value of `T` in-place.
  ///
  /// This will call the destructor of the contained value.
  template <typename... Args> auto emplace(Args... args) -> void {
    if (this->valid) {
      this->some.~T();
    }
    this->some = T{std::forward<Args>(args)...};
  }

  /// Takes the value out of the `Optional`, leaving an empty optional in its
  /// place.
  auto take() noexcept -> Optional<T> {
    if (this->valid) {
      this->valid = false;
      return Optional(std::move(this->some));
    }
    return Optional();
  }

  /// Destroys any contained value, but leaves the `Optional` intact.
  auto reset() noexcept
    requires(noexcept(~T()))
  {
    if (this->valid) {
      this->some.~T();
      this->valid = false;
    }
  }

  /// Destroys any contained value, but leaves the `Optional` intact.
  auto reset() {
    if (this->valid) {
      this->some.~T();
      this->valid = false;
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
      std::invocable<F, const T&>;
      { func(type) } -> std::same_as<U>;
    }
  {
    if (this->valid) {
      return func(this->some);
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
      std::invocable<F, T&>;
      { func(type) } -> std::same_as<U>;
    }
  {
    if (this->valid) {
      return func(this->some);
    }
    return Optional<U>();
  }

  // TODO: derive for `Cloneable<T>` or `Copyable<T>`
  //  - have a constexpr if to check for each
  //
  /// Clones the contained value.
  auto clone() const noexcept -> Optional<T>
    requires(Cloneable<T>)
  {
    if (!this->valid) {
      return Optional();
    }
    return Optional(this->val.clone());
  }

private:
  // TODO: Replace with std::variant
  union {
    /// Value for empty optional.
    bool none;

    /// The valid, contained value.
    T    some;
  };

  /// Determines if the optional is empty.
  bool valid;
};

} // namespace mu

#endif // !MU_OPTIONAL_H
