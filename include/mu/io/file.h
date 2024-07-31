#ifndef MU_FILE_H
#define MU_FILE_H

#include "mu/io/writer.h"  // Writer
#include "mu/primitives.h" // const_cstr, usize, u8
#include "mu/slice.h"      // Slice
#include <cassert>         // assert
#include <cstdarg>         // va_list
#include <cstdio>          // vfprintf, fwrite
#include <exception>       // exception
#include <unistd.h>        // dup

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

  explicit File()                          = default;
  File(const File& other)                  = delete;
  File&       operator=(const File& other) = delete;

  static auto fromRaw(FILE* file) -> File {
    File new_file{};
    new_file.file = file;
    return new_file;
  }

  explicit File(const_cstr filename, Mode mode) {
    std::FILE* file = std::fopen(filename, getFileMode(mode));
    if (file == nullptr) {
      int closed = std::fclose(file);
      assert(closed == 0);
      throw FileNotFound(filename);
    }
    this->file = file;
    this->mode = mode;
  }

  File(File&& other) {
    this->file = other.file;
    other.file = nullptr;
  }

  File& operator=(File&& other) {
    if (other.file == this->file) {
      return *this;
    }
    this->file = other.file;
    other.file = nullptr;
    return *this;
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

  /// Write formatted data into this file.
  auto formatV(const_cstr fmt, va_list args) -> void override {
    usize written = std::vfprintf(this->file, fmt, args);
    assert(written != 0);
  }

  auto toRaw() const -> std::FILE* { return this->file; }

  /// Clones the file.
  auto clone() const -> File {
    FILE* copied_file =
        fdopen(dup(fileno(this->file)), getFileMode(this->mode));
    return File::fromRaw(copied_file);
  }

private:
  std::FILE*  file = nullptr;
  Mode        mode = Mode::Read;

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

struct Stdout : public Writer {
  explicit Stdout() noexcept                            = default;
  ~Stdout() noexcept                                    = default;
  Stdout(const Stdout& other) noexcept                  = default;
  Stdout& operator=(const Stdout& other) noexcept       = default;
  Stdout(Stdout&& other) noexcept                       = default;
  Stdout&            operator=(Stdout&& other) noexcept = default;

  /// Write the buffer to `stdout`, returning how many bytes were written.
  [[nodiscard]] auto write(Slice<u8> buf) -> usize override {
    return std::fwrite(buf.ptr(), sizeof(u8), buf.len(), stdout);
  }

  /// Write formatted data to `stdout`.
  auto formatV(const_cstr fmt, va_list args) -> void override {
    usize written = std::vfprintf(stdout, fmt, args);
    assert(written != 0);
  }
};

struct Stderr : public Writer {
  explicit Stderr() noexcept                            = default;
  ~Stderr() noexcept                                    = default;
  Stderr(const Stderr& other) noexcept                  = default;
  Stderr& operator=(const Stderr& other) noexcept       = default;
  Stderr(Stderr&& other) noexcept                       = default;
  Stderr&            operator=(Stderr&& other) noexcept = default;

  /// Write the buffer to `stderr`, returning how many bytes were written.
  [[nodiscard]] auto write(Slice<u8> buf) -> usize override {
    return std::fwrite(buf.ptr(), sizeof(u8), buf.len(), stderr);
  }

  /// Write formatted data to `stderr`.
  auto formatV(const_cstr fmt, va_list args) -> void override {
    usize written = std::vfprintf(stderr, fmt, args);
    assert(written != 0);
  }
};

} // namespace mu::io

#endif // !MU_FILE_H
