#ifndef MU_DEBUGGABLE_H
#define MU_DEBUGGABLE_H

#include "mu/primitives.h" // u8, usize, cstr
#include <cassert>         // assert
#include <concepts>        // same_as
#include <cstdio>          // stdout, fprintf

namespace mu {
using namespace primitives;

template <typename T>
concept Debuggable = requires(const T self, u8* buf) {
  { self.writeToBuf(buf) } -> std::same_as<void>;
};

// template <Debuggable Context> struct Debug {
template <typename Context> struct Debug {
  auto debug() const -> void
    requires(Debuggable<Context>)
  {
    // TODO: Create new buffer

    //  - Call writeToBuf
    // Context* self = static_cast<Context*>(this);
    // const u8 buf[Context::maxWriteableSize];
    // cstr     str = reinterpret_cast<cstr>(buf);
    // self->writeToBuf(str);
    // usize written = std::fprintf(stdout, "%s", str);
    // assert(written != 0);
  }
};

} // namespace mu

#endif // !MU_DEBUGGABLE_H
