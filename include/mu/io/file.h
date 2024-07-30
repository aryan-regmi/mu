#ifndef MU_FILE_H
#define MU_FILE_H

#include "mu/io/writer.h"  // Writer
#include "mu/primitives.h" // const_cstr, usize, u8
#include "mu/slice.h"      // Slice
#include <cassert>         // assert
#include <cstdarg>         // va_list, va_start, va_end
#include <cstdio>          // fprintf, fwrite
#include <exception>

namespace mu::io {

/// File not found.
class FileNotFound : std::exception {
public:
  explicit FileNotFound(const_cstr filename) : filename{filename} {}

  /// Explains the error.
  auto what() const throw() -> const_cstr override {
    return "FileNotFound: The file was not found";
  }

  const_cstr filename;
};

/// A file.
class File : public Writer {
public:
  enum class Mode {
    Read,
    Write,
    Append,
    ReadExtended,
    WriteExtended,
    AppendExtended,
  };

  explicit File() = default;

  explicit File(const_cstr filename, Mode mode) {
    std::FILE* file = std::fopen(filename, getFileMode(mode));
    if (file == nullptr) {
      int closed = std::fclose(file);
      assert(closed == 0);
      throw FileNotFound(filename);
    }
    this->file = file;
  }

  ~File() {
    int closed = std::fclose(this->file);
    assert(closed == 0);
  }

  /// Write the buffer to this file, returning how many bytes were written.
  [[nodiscard]] auto write(Slice<u8> buf) -> usize override {
    return std::fwrite(buf.ptr(), sizeof(u8), buf.len(), this->file);
  }

  /// Writes a formatted string to this file.
  auto format(const_cstr fmt, ...) -> void override {
    std::va_list args;
    va_start(args, fmt);
    usize written = std::fprintf(this->file, fmt, args);
    assert(written != 0);
    va_end(args);
  };

private:
  std::FILE*  file = nullptr;

  static auto getFileMode(Mode mode) -> const_cstr {
    switch (mode) {
    case Mode::Read:
      return "r";
    case Mode::Write:
      return "w";
    case Mode::Append:
      return "a";
    case Mode::ReadExtended:
      return "r+";
    case Mode::WriteExtended:
      return "w+";
    case Mode::AppendExtended:
      return "a+";
    }
  }
};

} // namespace mu::io

#endif // !MU_FILE_H
