#include "mu/debuggable.h"

#include "mu/io/file.h"      // Stdout
#include "mu/io/formatter.h" // Formatter
#include "mu/primitives.h"   // f64, u8, const_cstr
#include <source_location>   // source_location

namespace mu {
auto dbg(u8 val, std::source_location loc) -> void {
  io::Formatter<io::Stdout> fmt{};
  fmt.format("[%s:%zu:%zu] = ", loc.file_name(), loc.line(), loc.column());
  fmt.format("%d\n", static_cast<int>(val));
}

auto dbg(u8* str, std::source_location loc) -> void {
  io::Formatter<io::Stdout> fmt{};
  fmt.format("[%s:%zu:%zu] = ", loc.file_name(), loc.line(), loc.column());
  fmt.format("\" %s \"\n", str);
}

auto dbg(const_cstr str, std::source_location loc) -> void {
  io::Formatter<io::Stdout> fmt{};
  fmt.format("[%s:%zu:%zu] = ", loc.file_name(), loc.line(), loc.column());
  fmt.format("\" %s \"\n", str);
}

auto dbg(int val, std::source_location loc) -> void {
  io::Formatter<io::Stdout> fmt{};
  fmt.format("[%s:%zu:%zu] = ", loc.file_name(), loc.line(), loc.column());
  fmt.format("%d\n", val);
}

// TODO: Handle precision?
auto dbg(f64 val, std::source_location loc) -> void {
  io::Formatter<io::Stdout> fmt{};
  fmt.format("[%s:%zu:%zu] = ", loc.file_name(), loc.line(), loc.column());
  fmt.format("%f\n", val);
}

} // namespace mu
