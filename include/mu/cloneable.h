#ifndef MU_CLONEABLE_H
#define MU_CLONEABLE_H

#include <concepts> // same_as
#include <type_traits>

// TODO: Move defs to `common.h`

namespace mu {

template <typename T>
concept Cloneable = requires(const T self) {
  { self.clone() } -> std::same_as<T>;
};

template <typename T>
concept Copyable = std::is_trivially_copyable_v<T>;

} // namespace mu

#endif // !MU_CLONEABLE_H
