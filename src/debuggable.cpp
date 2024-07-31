#include "mu/debuggable.h"

#include "mu/primitives.h" // f64, u8, const_cstr
#include <iostream>        // cout, endl
#include <source_location> // source_location

namespace mu {
auto dbg(u8 val, std::source_location loc) -> void {
  std::cout << "[" << loc.file_name() << ":" << loc.line() << ":"
            << loc.column() << "] = ";
  std::cout << static_cast<int>(val) << std::endl;
}

auto dbg(u8* str, std::source_location loc) -> void {
  std::cout << "[" << loc.file_name() << ":" << loc.line() << ":"
            << loc.column() << "] = ";
  std::cout << "\"" << str << "\"" << std::endl;
}

auto dbg(const_cstr str, std::source_location loc) -> void {
  std::cout << "[" << loc.file_name() << ":" << loc.line() << ":"
            << loc.column() << "] = ";
  std::cout << "\"" << str << "\"" << std::endl;
}

auto dbg(int val, std::source_location loc) -> void {
  std::cout << "[" << loc.file_name() << ":" << loc.line() << ":"
            << loc.column() << "] = ";
  std::cout << val << std::endl;
}

auto dbg(f64 val, std::source_location loc) -> void {
  std::cout << "[" << loc.file_name() << ":" << loc.line() << ":"
            << loc.column() << "] = ";
  std::cout << val << std::endl;
}

} // namespace mu
