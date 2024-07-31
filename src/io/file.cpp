#include "mu/io/file.h"

#include "mu/primitives.h" // const_cstr, usize, u8
#include "mu/slice.h"      // Slice
#include <cassert>         // assert
#include <cstdarg>         // va_list
#include <cstdio>          // vfprintf, fwrite
#include <unistd.h>        // dup

namespace mu::io {

auto FileNotFound::what() const throw() -> const_cstr {
  return "FileNotFound: The file was not found";
}

auto File::fromRaw(FILE* file) -> File {
  File new_file{};
  new_file.file = file;
  return new_file;
}

File::File(const_cstr filename, Mode mode) {
  std::FILE* file = std::fopen(filename, getFileMode(mode));
  if (file == nullptr) {
    int closed = std::fclose(file);
    assert(closed == 0);
    throw FileNotFound(filename);
  }
  this->file = file;
  this->mode = mode;
}

File::File(File&& other) {
  this->file = other.file;
  other.file = nullptr;
}

File& File::operator=(File&& other) {
  if (other.file == this->file) {
    return *this;
  }
  this->file = other.file;
  other.file = nullptr;
  return *this;
}

File::~File() {
  if ((this->file != stdout) && (this->file != stderr)) {
    if (this->file != nullptr) {
      int closed = std::fclose(this->file);
      assert((closed == 0) || (closed == EOF));
    }
  }
}

[[nodiscard]] auto File::write(Slice<u8> buf) -> usize {
  return std::fwrite(buf.ptr(), sizeof(u8), buf.len(), this->file);
}

auto File::formatV(const_cstr fmt, va_list args) -> void {
  usize written = std::vfprintf(this->file, fmt, args);
  assert(written != 0);
}

auto File::toRaw() const -> std::FILE* { return this->file; }

auto File::clone() const -> File {
  FILE* copied_file = fdopen(dup(fileno(this->file)), getFileMode(this->mode));
  return File::fromRaw(copied_file);
}

auto File::getFileMode(Mode mode) -> const_cstr {
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
  default:
    return "r";
  }
}

[[nodiscard]] auto Stdout::write(Slice<u8> buf) -> usize {
  return std::fwrite(buf.ptr(), sizeof(u8), buf.len(), stdout);
}

auto Stdout::formatV(const_cstr fmt, va_list args) -> void {
  usize written = std::vfprintf(stdout, fmt, args);
  assert(written != 0);
}

[[nodiscard]] auto Stderr::write(Slice<u8> buf) -> usize {
  return std::fwrite(buf.ptr(), sizeof(u8), buf.len(), stderr);
}

auto Stderr::formatV(const_cstr fmt, va_list args) -> void {
  usize written = std::vfprintf(stderr, fmt, args);
  assert(written != 0);
}

} // namespace mu::io
