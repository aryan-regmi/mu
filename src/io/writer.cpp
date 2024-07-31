#include "mu/io/writer.h"

#include "mu/primitives.h" // u8, usize, const_cstr
#include <cstdarg>         // va_list, va_start, va_end

namespace mu::io {

auto Writer::format(const_cstr fmt, ...) -> void {
  va_list args;
  va_start(args, fmt);
  this->formatV(fmt, args);
  va_end(args);
}

auto Writer::writeAll(Slice<u8> buf) -> void {
  usize idx = 0;
  while (idx != buf.len()) {
    idx += this->write(buf);
    buf  = Slice(buf.ptr() + idx, buf.len());
  }
}

} // namespace mu::io
