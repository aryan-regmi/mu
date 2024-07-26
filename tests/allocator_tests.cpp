#include "mu/mem/c_allocator.h"
#include "mu/slice.h"
#include <cassert>
#include <cstdio>

struct Tst {
  int  x;
  int  y;
  int  z;
  bool b;
};

int main(void) {
  using namespace mu;
  mu::mem::CAllocator allocator{};

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
    Slice<int>   val       = allocator.allocAligned<int>(1, alignment);
    u8*          aligned   = reinterpret_cast<u8*>(val.ptr());
    u8*          offset    = aligned - alignment;
    u8*          alloced   = aligned - *offset;
    assert((reinterpret_cast<u8*>(val.ptr()) - alloced) == alignment);
    allocator.free(val);
  }

  return 0;
}
