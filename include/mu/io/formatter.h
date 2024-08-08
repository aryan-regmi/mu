#ifndef MU_FORMATTER_H
#define MU_FORMATTER_H

#include "mu/io/writer.h"
#include "mu/primitives.h"
#include <utility>
namespace mu::io {

template <Writeable T> class Formatter : public Writer {
  Formatter() = default;

  static auto fromRaw(T writer, usize padding = 0) -> Formatter {
    Formatter fmt{};
    fmt.writer  = writer;
    fmt.padding = padding;
  }

  template <typename... Args>
  explicit Formatter(usize padding = 0, Args... args)
      : writer{std::forward<Args>(args)...}, padding{padding} {}

  auto pad() -> void {
    for (usize i = 0; i < this->padding; i++) {
      this->writer.write(Slice<u8>("\t"));
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
