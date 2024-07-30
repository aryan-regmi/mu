#ifndef MU_WRITEABLE_H
#define MU_WRITEABLE_H

#include "mu/mem/buffer.h" // Buffer
#include "mu/mem/mem.h"
#include "mu/panic.h"      // MU_PANIC
#include "mu/primitives.h" // u8, usize, cstr
#include <bit>
#include <cassert> // assert
#include <cstdio>  // FILE, fprintf, sprintf

// TODO: Make thread safe version!
namespace mu {
using namespace primitives;

class Writer {
public:
  Writer()          = default;
  virtual ~Writer() = default;

  /// Write the buffer into this writer, returning how many bytes were written.
  [[nodiscard]] auto write(mem::Buffer buf) -> usize {
    return this->write_fn(this->buf, buf);
  }

  /// Attempts to write an entire buffer into this writer.
  auto writeAll(mem::Buffer buf) -> void {
    usize idx = 0;
    while (idx != buf.len) {
      idx     += this->write(buf);
      buf.ptr += idx;
    }
  }

  /// Writes a formatted string into this writer.
  template <typename... Args>
  auto format(const_cstr fmt, Args... args) -> void {
    assert(this->buf.len > std::strlen(fmt) + sizeof...(args));
    usize written = std::snprintf(this->buf.ptr, this->buf.len, fmt, args...);
    assert(written != 0);
  }

  /// Writes to the specified file.
  auto writeToFile(std::FILE* file) const {
    usize written = std::fprintf(file, "%s", this->buf.ptr);
    assert(written != 0);
  }

  /// Write the object into this writer.
  template <typename T>
  auto writeObject(T obj, usize bytesize = sizeof(T)) -> void {
    u8* bytes = reinterpret_cast<u8*>(reinterpret_cast<char*>(&obj));
    this->writeAll(mem::Buffer(bytes, bytesize));
  }

  /// Write the object into this writer, with the endianness specified in the
  /// template argument (`Endian`).
  template <typename T, std::endian Endian>
  auto writeObjectEndian(T obj) -> void {
    if constexpr (std::endian::native == Endian) {
      this->writeObject(obj);
    } else if constexpr (std::endian::native != Endian) {
      mem::swapEndian<T>(obj);
      this->writeObject(obj);
    }
  }

protected:
  mem::Buffer buf;

private:
  virtual auto write_fn(mem::Buffer /*buf*/, mem::Buffer /*bytes*/) -> usize {
    MU_PANIC("`write_fn` not implemented!");
    return 0;
  }
};

} // namespace mu

#endif // !MU_WRITEABLE_H
