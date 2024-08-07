#ifndef MU_CLONEABLE_H
#define MU_CLONEABLE_H

#include <concepts> // same_as
#include <type_traits>

// TODO: Move defs to `common.h`

namespace mu {

namespace internal {
/// Determines if `T` has a `cloneImpl` method.
template <typename T>
concept HasCloneImpl = requires(const T self) {
  { self.cloneImpl() } -> std::same_as<T>;
};
} // namespace internal

/// Determines if `T` is trivially copyable (implicit, bit-wise copy).
template <typename T>
concept Copyable = std::is_trivially_copyable_v<T>;

/// Determines if `T` cloneable (explicit copy).
template <typename T>
concept Cloneable = Copyable<T> || internal::HasCloneImpl<T>;

/// Mixin providing the `clone` method for explicit copies.
template <Cloneable Context> class Clone {
  auto clone() -> Context {
    const Context* self = static_cast<const Context*>(this);
    if constexpr (Copyable<Context>) {
      return *self;
    } else {
      return self->cloneImpl();
    }
  }
};

} // namespace mu

#endif // !MU_CLONEABLE_H
