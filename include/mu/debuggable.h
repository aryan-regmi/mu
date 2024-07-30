#ifndef MU_DEBUGGABLE_H
#define MU_DEBUGGABLE_H

#include <cassert>  // assert
#include <concepts> // same_as
#include <cstdio>   // stdout, fprintf

namespace mu {

template <typename T> class Slice;

template <typename T>
concept Debuggable = requires(const T self, Slice<T> buf) {
  { self.writeToBuf(buf) } -> std::same_as<void>;
};

// template <Debuggable Context> struct Debug {
template <typename Context> struct Debug {
  auto debug() const -> void
    requires(Debuggable<Context>)
  {
    // TODO: Impl!
  }
};

} // namespace mu

#endif // !MU_DEBUGGABLE_H
