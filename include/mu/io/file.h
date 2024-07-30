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

enum class FileMode {
  Read,
  Write,
  Append,
  ReadExtended,
  WriteExtended,
  AppendExtended,
};

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

class File : public Writer {
public:
  explicit File() = default;

  explicit File(const_cstr filename, FileMode mode) {
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

  static auto getFileMode(FileMode mode) -> const_cstr {
    switch (mode) {
    case FileMode::Read:
      return "r";
    case FileMode::Write:
      return "w";
    case FileMode::Append:
      return "a";
    case FileMode::ReadExtended:
      return "r+";
    case FileMode::WriteExtended:
      return "w+";
    case FileMode::AppendExtended:
      return "a+";
    }
  }
};

} // namespace mu::io

#endif // !MU_FILE_H
