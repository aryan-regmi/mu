#ifndef MU_FILE_H
#define MU_FILE_H

#include "mu/io/writer.h"
#include "mu/primitives.h"
#include "mu/slice.h"
#include <cstdio>

namespace mu::io {

class File : Writer {
public:
  /// Write the buffer to this file, returning how many bytes were written.
  [[nodiscard]] auto write(Slice<u8> buf) -> usize override {
    return std::fwrite(buf.ptr(), sizeof(u8), buf.len(), this->file);
  }

private:
  std::FILE* file;
};

} // namespace mu::io

#endif // !MU_FILE_H
