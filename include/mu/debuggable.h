#ifndef MU_DEBUGGABLE_H
#define MU_DEBUGGABLE_H

#include "mu/io/file.h"
#include "mu/io/writer.h" // Writer
#include "mu/primitives.h"
#include <cassert>  // assert
#include <concepts> // same_as
#include <cstdio>   // stdout, fprintf
#include <iostream>
#include <source_location>

namespace mu {

template <typename T>
concept HasDebugFn = requires(const T self) {
  { self.debug() } -> std::same_as<void>;
};

template <typename T>
concept Debuggable = requires(const T self, io::Writer& writer) {
  { self.writeFmt(writer) } -> std::same_as<void>;
};

// template <Debuggable Context> struct Debug {
template <typename Context> struct Debug {
  auto debug() const -> void
    requires(Debuggable<Context>)
  {
    const Context*       self      = static_cast<const Context*>(this);
    FILE*                file      = stdout;
    io::File             writer    = io::File(file);
    io::ThreadSafeWriter ts_writer = io::ThreadSafeWriter(io::File(file));
    self->writeFmt(ts_writer);
  }
};

template <typename T>
auto dbg(T                    val,
         std::source_location loc = std::source_location::current()) -> void
  requires(HasDebugFn<T>)
{
  std::cout << "[" << loc.file_name() << ":" << loc.line() << ":"
            << loc.column() << "] = ";
  val.debug();
  std::cout << std::endl;
}

// TODO: Put impls in cpp file

auto dbg(u8                   val,
         std::source_location loc = std::source_location::current()) -> void {
  std::cout << "[" << loc.file_name() << ":" << loc.line() << ":"
            << loc.column() << "] = ";
  std::cout << static_cast<int>(val) << std::endl;
}

auto dbg(u8*                  str,
         std::source_location loc = std::source_location::current()) -> void {
  std::cout << "[" << loc.file_name() << ":" << loc.line() << ":"
            << loc.column() << "] = ";
  std::cout << "\"" << str << "\"" << std::endl;
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
