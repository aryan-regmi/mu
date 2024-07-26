#include "mu/mem/allocator.h"
#include "mu/mem/c_allocator.h"
#include "mu/primitives.h"
#include "mu/slice.h"
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

  Tst*                val1 = allocator.create<Tst>();
  int*                val2 = allocator.create<int>();
  Slice<int>          val3 = allocator.allocAligned<int>(1, alignof(int));
  allocator.destroy(val1);
  allocator.destroy(val2);
  allocator.free(val3);

  return 0;
}
