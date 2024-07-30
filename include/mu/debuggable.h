#ifndef MU_DEBUGGABLE_H
#define MU_DEBUGGABLE_H

#include "mu/primitives.h" // u8
#include <cassert>         // assert
#include <concepts>        // same_as
#include <cstdio>          // stdout, fprintf

namespace mu {
using namespace primitives;

template <typename T> class Slice;

template <typename T>
concept Debuggable = requires(const T self, Slice<u8> buf) {
  { self.writeToBuf(buf) } -> std::same_as<void>;
};

// template <Debuggable Context> struct Debug {
template <typename Context> struct Debug {
  auto debug() const -> void
    requires(Debuggable<Context>)
  {
    // this->writeToBuf(Slice<u8>(nullptr, 0));
    // TODO: Impl!
  }
};

} // namespace mu

#endif // !MU_DEBUGGABLE_H
