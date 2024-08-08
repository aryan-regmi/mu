#ifndef MU_DEBUGGABLE_H
#define MU_DEBUGGABLE_H

#include "mu/io/file.h"      // Stdout
#include "mu/io/formatter.h" // Formatter
#include "mu/io/writer.h"    // Writer
#include "mu/primitives.h"   // f64, u8, const_cstr
#include <cassert>           // assert
#include <concepts>          // same_as
#include <iostream>          // cout, endl
#include <source_location>   // source_location

namespace mu {

namespace internal {
template <class T>
concept HasEmptyDebugFn = requires(const T self) {
  { self.debug() } -> std::same_as<void>;
};

template <class T>
concept HasFmtDebugFn = requires(const T self, io::Formatter<io::Stdout>& fmt) {
  { self.debug(fmt) } -> std::same_as<void>;
};
} // namespace internal

/// Concept to check if `T` has `void debug() const` method.
template <class T>
concept HasDebugFn = internal::HasEmptyDebugFn<T> || internal::HasFmtDebugFn<T>;

/// Concept to check if `T` has `void writeFmt(io::Writer&) const` method.
template <class T>
concept Debuggable = requires(const T self, io::Formatter<io::Stdout>& fmt) {
  { self.writeFmt(fmt) } -> std::same_as<void>;
};

/// Mixin that provides debugging functionality to types that satisfy the
/// `Debuggable` constraint.
template <class Context> struct Debug {
  auto debug(io::Formatter<io::Stdout>& fmt) const -> void
    requires(Debuggable<Context>)
  {
    const Context* self = static_cast<const Context*>(this);
    self->writeFmt(fmt);
  }
};

/// Debugs type `Slice<T>` by calling its `debug()` method.
template <class T>
auto dbg(Slice<T>             val,
         std::source_location loc = std::source_location::current()) -> void {
  std::cout << "[" << loc.file_name() << ":" << loc.line() << ":"
            << loc.column() << "] = ";
  val.debug();
  std::cout << std::endl;
}

/// Debugs type `T` by calling its `debug()` method.
template <class T>
auto dbg(T                    val,
         std::source_location loc = std::source_location::current()) -> void
  requires(HasDebugFn<T>)
{
  std::cout << "[" << loc.file_name() << ":" << loc.line() << ":"
            << loc.column() << "] = ";
  io::Formatter<io::Stdout> fmt{};
  val.debug(fmt);
  std::cout << std::endl;
}

/// Debugs a `u8` value.
auto dbg(u8                   val,
         std::source_location loc = std::source_location::current()) -> void;

/// Debugs a byte pointer (u8*) as a C-string.
auto dbg(u8*                  str,
         std::source_location loc = std::source_location::current()) -> void;

/// Debugs a C-string.
auto dbg(const_cstr           str,
         std::source_location loc = std::source_location::current()) -> void;

/// Debugs an integer.
auto dbg(int                  val,
         std::source_location loc = std::source_location::current()) -> void;

/// Debugs a float.
auto dbg(f64                  val,
         std::source_location loc = std::source_location::current()) -> void;

// TODO: Add `dbg` for arrays!

} // namespace mu

#endif // !MU_DEBUGGABLE_H
