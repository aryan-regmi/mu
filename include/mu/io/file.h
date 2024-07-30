#ifndef MU_FILE_H
#define MU_FILE_H

#include "mu/io/writer.h"  // Writer
#include "mu/primitives.h" // const_cstr, usize, u8
#include "mu/slice.h"      // Slice
#include <cassert>         // assert
#include <cstdarg>         // va_list
#include <cstdio>          // vfprintf, fwrite
#include <exception>       // exception

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

  explicit File(FILE* file) { this->file = file; }

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
    if ((this->file != stdout) && (this->file != stderr)) {
      if (this->file != nullptr) {
        int closed = std::fclose(this->file);
        assert((closed == 0) || (closed == EOF));
      }
    }
  }

  /// Write the buffer to this file, returning how many bytes were written.
  [[nodiscard]] auto write(Slice<u8> buf) -> usize override {
    return std::fwrite(buf.ptr(), sizeof(u8), buf.len(), this->file);
  }

  auto formatV(const_cstr fmt, va_list args) -> void override {
    usize written = std::vfprintf(this->file, fmt, args);
    assert(written != 0);
  }

  auto toRaw() const -> std::FILE* { return this->file; }

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
