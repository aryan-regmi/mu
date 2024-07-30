#ifndef MU_C_WRITER_H
#define MU_C_WRITER_H

#include "mu/io/writer.h"  // Writer
#include "mu/primitives.h" // u8, usize, cstr
#include <cstdio>          // FILE

namespace mu::io {
using namespace primitives;

class CWriter : public Writer {
  explicit CWriter() = default;
  ~CWriter()         = default;

  explicit CWriter(FILE* file) : file{file} {
    // this->buf = mem::Buffer(reinterpret_cast<u8*>(file))
  }

private:
  FILE* file = nullptr;

  auto  write_fn(mem::Buffer /*buf*/, mem::Buffer /*bytes*/) -> usize override {
    MU_PANIC("`write_fn` not implemented!");
    return 0;
  }
};

} // namespace mu::io

#endif // !MU_C_WRITER_H
