#ifndef MU_FORMATTER_H
#define MU_FORMATTER_H

#include "mu/io/writer.h"
#include "mu/primitives.h"
#include <utility>
namespace mu::io {

template <Writeable T> class Formatter : public Writer {
  Formatter() = default;

  static auto fromRaw(T writer, usize padding = 1) -> Formatter {
    Formatter fmt{};
    fmt.writer  = writer;
    fmt.padding = padding;
  }

  template <typename... Args>
  explicit Formatter(usize padding = 1, Args... args)
      : writer{std::forward<Args>(args)...}, padding{padding} {}

  auto pad() -> void {
    const_cstr tab    = "\t";
    const u8*  tab_u8 = reinterpret_cast<const u8*>(tab);
    Slice<u8>  buf    = Slice(const_cast<u8*>(tab_u8), 1);
    for (usize i = 0; i < this->padding; i++) {
      this->writer.write(buf);
    }
  }

  auto write(Slice<u8> buf) -> usize override {
    this->pad();
    this->writer.write(buf);
  }

private:
  T     writer;
  usize padding;

  auto  formatV(const_cstr fmt, va_list args) -> void override {
    this->pad();
    this->writer.formatV(fmt, args);
  }
};

} // namespace mu::io

#endif // !MU_FORMATTER_H
