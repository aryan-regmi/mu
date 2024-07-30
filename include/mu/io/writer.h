#ifndef MU_WRITER_H
#define MU_WRITER_H

#include "mu/mem/utils.h"  // swapEndian
#include "mu/primitives.h" // u8, usize, cstr
#include "mu/slice.h"      // Slice
#include <bit>             // endian::native
#include <cassert>         // assert
#include <cstdio>          // FILE, fprintf, snprintf

// TODO: Make thread-safe writer:
//  - Same idea as thread-safe allocator
//  - Just wrap the `write` and `format` functions in locks
//  ```
//   class ThreadSafeWriter {
//    public:
//      auto write(Slice<u8> buf) -> usize {
//        this->mutex.lock();
//        this->writer.write(buf);
//        this->mutex.unlock();
//      }
//      auto format(const_cstr fmt, ...) -> void {
//        this->mutex.lock();
//        this->writer.fmt(fmt, ...);
//        this->mutex.unlock();
//      }
//    private:
//      Mutex mutex;
//      Writer& writer;
//   }
//  ```
// https://stackoverflow.com/questions/357307/how-to-call-a-parent-class-function-from-derived-class-function

namespace mu::io {

class Writer {
public:
  Writer()                                                           = default;
  virtual ~Writer()                                                  = default;

  /// Write the buffer into this writer, returning how many bytes were written.
  [[nodiscard]] virtual auto write(Slice<u8> /*buf*/) -> usize       = 0;

  // TODO: Make this a parameter pack
  //
  /// Writes a formatted string into this writer.
  virtual auto               format(const_cstr /*fmt*/, ...) -> void = 0;

  /// Attempts to write an entire buffer into this writer.
  auto                       writeAll(Slice<u8> buf) -> void {
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

} // namespace mu::io

#endif // !MU_WRITER_H
