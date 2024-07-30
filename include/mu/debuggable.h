#ifndef MU_DEBUGGABLE_H
#define MU_DEBUGGABLE_H

#include "mu/io/writer.h" // Writer
#include "mu/primitives.h"
#include <cassert>  // assert
#include <concepts> // same_as
#include <cstdio>   // stdout, fprintf
#include <iostream>
#include <source_location>

namespace mu {

template <typename T>
concept HasDebugFn = requires(const T self, const std::source_location loc) {
  { self.debug(loc) } -> std::same_as<void>;
};

template <typename T>
concept Debuggable = requires(const T self, io::Writer& writer) {
  { self.write(writer) } -> std::same_as<void>;
};

// template <Debuggable Context> struct Debug {
template <typename Context> struct Debug {
  auto debug(const std::source_location loc =
                 std::source_location::current()) const -> void
    requires(Debuggable<Context>)
  {
    // TODO: Impl!
    //  - Create File(stdout) writer
    //  - Write the self.write() value to that!
  }
};

template <typename T>
auto dbg(T                    val,
         std::source_location loc = std::source_location::current()) -> void
  requires(HasDebugFn<T>)
{
  val.debug(loc);
}

// TODO: Put impls in cpp file

auto dbg(u8                   val,
         std::source_location loc = std::source_location::current()) -> void {
  std::cout << "[" << loc.file_name() << ":" << loc.line() << ":"
            << loc.column() << "] = ";
  std::cout << static_cast<int>(val) << std::endl;
}

auto dbg(const_cstr           str,
         std::source_location loc = std::source_location::current()) -> void {
  std::cout << "[" << loc.file_name() << ":" << loc.line() << ":"
            << loc.column() << "] = ";
  std::cout << "\"" << str << "\"" << std::endl;
}

auto dbg(int                  val,
         std::source_location loc = std::source_location::current()) -> void {
  std::cout << "[" << loc.file_name() << ":" << loc.line() << ":"
            << loc.column() << "] = ";
  std::cout << val << std::endl;
}

auto dbg(f64                  val,
         std::source_location loc = std::source_location::current()) -> void {
  std::cout << "[" << loc.file_name() << ":" << loc.line() << ":"
            << loc.column() << "] = ";
  std::cout << val << std::endl;
}

} // namespace mu

#endif // !MU_DEBUGGABLE_H
