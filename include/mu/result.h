#ifndef MU_RESULT_H
#define MU_RESULT_H

#include <algorithm>
#include <utility>
#include <variant>

namespace mu {

template <typename T> struct Ok {
  Ok()                                    = delete;
  Ok(Ok&& other) noexcept                 = default;
  Ok(const Ok& other) noexcept            = default;
  Ok& operator=(const Ok& other) noexcept = default;
  Ok& operator=(Ok&& other) noexcept      = default;

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

  T val;
};

template <typename E> struct Err {
  Err()                                     = delete;
  Err(Err&& other) noexcept                 = default;
  Err(const Err& other) noexcept            = default;
  Err& operator=(const Err& other) noexcept = default;
  Err& operator=(Err&& other) noexcept      = default;

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

  E err;
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
      Ok<T> val = std::get(this->val);
      return val.val;
    }
  }

  auto unwrap() -> T& {
    if (this->isOk()) {
      Ok<T> val = std::get(this->val);
      return val.val;
    }
  }

private:
  std::variant<Ok<T>, Err<E>> val;
};

} // namespace mu

#endif // !MU_RESULT_H
