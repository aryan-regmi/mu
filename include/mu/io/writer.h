#ifndef MU_WRITER_H
#define MU_WRITER_H

#include "mu/mem/utils.h"  // swapEndian
#include "mu/primitives.h" // u8, usize, cstr
#include "mu/slice.h"      // Slice
#include <bit>             // endian::native
#include <cassert>         // assert
#include <cstdarg>         // va_list, va_start, va_end
#include <cstdio>          // FILE, fprintf, snprintf
#include <mutex>           // mutex, lock_guard

namespace mu::io {

class Writer {
public:
  Writer()                                                     = default;
  virtual ~Writer()                                            = default;

  /// Write the buffer into this writer, returning how many bytes were written.
  [[nodiscard]] virtual auto write(Slice<u8> /*buf*/) -> usize = 0;

  /// Write a formatted string into the writer.
  virtual auto               formatV(const_cstr fmt, va_list args) -> void = 0;

  /// Writes a formatted string into this writer.
  auto                       format(const_cstr fmt, ...) -> void {
    va_list args;
    va_start(args, fmt);
    this->formatV(fmt, args);
    va_end(args);
  }

  /// Attempts to write an entire buffer into this writer.
  auto writeAll(Slice<u8> buf) -> void {
    usize idx = 0;
    while (idx != buf.len()) {
      idx += this->write(buf);
      buf  = Slice(buf.ptr() + idx, buf.len());
    }
  }

  /// Write the object into this writer.
  template <typename T>
  auto writeObject(T obj, usize bytesize = sizeof(T)) -> void {
    u8* bytes = reinterpret_cast<u8*>(reinterpret_cast<char*>(&obj));
    this->writeAll(Slice(bytes, bytesize));
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
};

template <typename T>
concept Writeable =
    requires(T self, Slice<u8> buf, const_cstr fmt, va_list args) {
      { self.write(buf) } -> std::same_as<usize>;
      { self.formatV(fmt, args) } -> std::same_as<void>;
    };

/// A thread-safe `Writer`.
///
/// This locks a mutex before the `write` and `formatV` calls to ensure thread
/// safety.
template <Writeable T> class ThreadSafeWriter : public Writer {
public:
  ~ThreadSafeWriter() = default;

  explicit ThreadSafeWriter(T writer) : writer{writer} {}

  auto write(Slice<u8> buf) -> usize override {
    const std::lock_guard<std::mutex> lock(this->mutex);
    usize                             written = this->writer.write(buf);
    return written;
  }

private:
  std::mutex mutex;
  T          writer;

  auto       formatV(const_cstr fmt, va_list args) -> void override {
    const std::lock_guard<std::mutex> lock(this->mutex);
    this->writer.formatV(fmt, args);
  }
};

} // namespace mu::io

#endif // !MU_WRITER_H
