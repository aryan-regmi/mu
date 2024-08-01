#ifndef MU_CLONEABLE_H
#define MU_CLONEABLE_H

#include <concepts> // same_as

namespace mu {

// TODO: Add `Copy` trait for trivially_copyable types

template <typename T>
concept Cloneable = requires(const T self) {
  { self.clone() } -> std::same_as<T>;
};

} // namespace mu

#endif // !MU_CLONEABLE_H
