#ifndef MU_CLONEABLE_H
#define MU_CLONEABLE_H

#include <concepts> // same_as
#include <type_traits>

// TODO: Move defs to `common.h`

namespace mu {

namespace internal {
template <typename T>
concept HasCloneFn = requires(const T self) {
  { self.clone() } -> std::same_as<T>;
};
} // namespace internal

template <typename T>
concept Copyable = std::is_trivially_copyable_v<T>;

template <typename T>
concept Cloneable = Copyable<T> || internal::HasCloneFn<T>;

} // namespace mu

#endif // !MU_CLONEABLE_H
