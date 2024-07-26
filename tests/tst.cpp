#include "mu/mem.h"
#include "mu/primitives.h"
#include <cstdio>

int main(void) {
  using namespace mu::primitives;
  const_cstr num = "hi";
  printf("Hello World! %s", num);

  mu::mem::Allocator allocator;
  int*               val = allocator.create<int>().unwrap();
  printf("Tst: %d", *val);

  return 0;
}
