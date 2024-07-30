#ifndef MU_DEBUGGABLE_H
#define MU_DEBUGGABLE_H

#include "mu/io/writer.h" // Writer
#include <cassert>        // assert
#include <concepts>       // same_as
#include <cstdio>         // stdout, fprintf

namespace mu {

template <typename T>
concept Debuggable = requires(const T self, io::Writer& writer) {
  { self.write(writer) } -> std::same_as<void>;
};

// template <Debuggable Context> struct Debug {
template <typename Context> struct Debug {
  auto debug() const -> void
    requires(Debuggable<Context>)
  {
    // TODO: Impl!
    //  - Create File(stdout) writer
    //  - Write the self.write() value to that!
  }
};

} // namespace mu

#endif // !MU_DEBUGGABLE_H
