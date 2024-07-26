#include "mu/mem/allocator.h"
#include "mu/mem/c_allocator.h"
#include "mu/primitives.h"
#include <cstdio>

int main(void) {
  using namespace mu::primitives;

  mu::mem::CAllocator allocator;
  int*                val = allocator.create<int>();
  *val                    = 2;
  printf("Tst: %d", *val);
  allocator.destroy(val);

  return 0;
}
