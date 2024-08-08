#include "mu/common.h"
#include "mu/debuggable.h"
#include "mu/mem/c_allocator.h"
#include "mu/primitives.h"
#include "mu/slice.h"
#include <cassert>
#include <cstdio>

using namespace mu;

struct Tst {
  int  x;
  int  y;
  int  z;
  bool b;
};

struct Dbgl : Debug<Dbgl> {
  auto writeFmt(io::Formatter<io::Stdout>& fmt) const -> void {
    fmt.format("Dbgl { val = %d }", this->val);
  }

  static constexpr const usize NUM = 42;
  int                          val = NUM;
};

int main(void) {
  mem::CAllocator allocator{};

  {
    Tst* val     = allocator.create<Tst>();
    u8*  aligned = reinterpret_cast<u8*>(val);
    u8*  offset  = aligned - alignof(Tst);
    u8*  alloced = aligned - *offset;
    assert((reinterpret_cast<u8*>(val) - alloced) == alignof(Tst));
    allocator.destroy(val);
  }

  {
    int* val     = allocator.create<int>();
    u8*  aligned = reinterpret_cast<u8*>(val);
    u8*  offset  = aligned - alignof(int);
    u8*  alloced = aligned - *offset;
    assert((reinterpret_cast<u8*>(val) - alloced) == alignof(int));
    allocator.destroy(val);
  }

  {
    constexpr u8 alignment = 16;
    Slice<int>   val       = allocator.allocAligned<int>(2, alignment);
    u8*          aligned   = reinterpret_cast<u8*>(val.ptr());
    u8*          offset    = aligned - alignment;
    u8*          alloced   = aligned - *offset;
    assert((reinterpret_cast<u8*>(val.ptr()) - alloced) == alignment);

    auto xxx = Dbgl{};
    dbg(xxx);

    // Testing slice
    try {
      val[0] = 1;
      val[1] = 2;
      dbg(val);
      assert(val[0] == 1);         // Doesn't throw
      assert(val[1] == 2);         // Doesn't throw
      printf("Error: %d", val[3]); // Throws error
    } catch (common::IndexOutOfBounds& e) {
      const usize BUFSIZE = 128;
      u8          buf[BUFSIZE];
      cstr        str     = reinterpret_cast<cstr>(buf);
      usize       written = std::sprintf(str, "%s (index: %zu, length: %zu)",
                                         e.what(), e.idx, e.len);
      written             = fprintf(stderr, "%s", str);
      assert(written != 0);
    }
    allocator.free(val);
  }

  return 0;
}
