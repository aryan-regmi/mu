#ifndef MU_FILE_H
#define MU_FILE_H

#include "mu/io/writer.h"  // Writer
#include "mu/primitives.h" // const_cstr, usize, u8
#include "mu/slice.h"      // Slice
#include <cstdarg>         // va_list
#include <exception>       // exception

namespace mu::io {

/// File not found.
class FileNotFound : std::exception {
public:
  explicit FileNotFound(const_cstr filename) : filename{filename} {}

  /// Explains the error.
  auto       what() const throw() -> const_cstr override;

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

  /// Creates a `File` from a raw `FILE*`.
  static auto fromRaw(FILE* file) -> File;

  /// Create/open the file with `filename` in the specified mode.
  explicit File(const_cstr filename, Mode mode);

  // TODO: Handle path, etc instead of raw filenames

  /// Move construct from `other`.
  File(File&& other) noexcept;

  /// Move assign from `other`.
  File& operator=(File&& other) noexcept;

  /// Cleanup resources used by `File`.
  ~File();

  /// Write the buffer to this file, returning how many bytes were written.
  [[nodiscard]] auto write(Slice<u8> buf) -> usize override;

  /// Write formatted data into this file.
  auto               formatV(const_cstr fmt, va_list args) -> void override;

  /// Get the raw `FILE*`.
  auto               toRaw() const -> std::FILE*;

  /// Clones the file.
  auto               clone() const -> File;

private:
  std::FILE*  file = nullptr;
  Mode        mode = Mode::Read;

  /// Get the file mode as a C-string.
  static auto getFileMode(Mode mode) -> const_cstr;
};

struct Stdout : public Writer {
  explicit Stdout() noexcept                            = default;
  ~Stdout() noexcept                                    = default;
  Stdout(const Stdout& other) noexcept                  = default;
  Stdout& operator=(const Stdout& other) noexcept       = default;
  Stdout(Stdout&& other) noexcept                       = default;
  Stdout&            operator=(Stdout&& other) noexcept = default;

  /// Write the buffer to `stdout`, returning how many bytes were written.
  [[nodiscard]] auto write(Slice<u8> buf) -> usize override;

  /// Write formatted data to `stdout`.
  auto               formatV(const_cstr fmt, va_list args) -> void override;
};

struct Stderr : public Writer {
  explicit Stderr() noexcept                            = default;
  ~Stderr() noexcept                                    = default;
  Stderr(const Stderr& other) noexcept                  = default;
  Stderr& operator=(const Stderr& other) noexcept       = default;
  Stderr(Stderr&& other) noexcept                       = default;
  Stderr&            operator=(Stderr&& other) noexcept = default;

  /// Write the buffer to `stderr`, returning how many bytes were written.
  [[nodiscard]] auto write(Slice<u8> buf) -> usize override;

  /// Write formatted data to `stderr`.
  auto               formatV(const_cstr fmt, va_list args) -> void override;
};

} // namespace mu::io

#endif // !MU_FILE_H
