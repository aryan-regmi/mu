#include "mu/common.h"
#include "mu/io/file.h"
#include "mu/mem/c_allocator.h"
#include "mu/mem/unique_ptr.h"
#include "mu/panic.h"
#include "mu/primitives.h"
#include <algorithm>
#include <cassert>
#include <cstdio>
#include <exception>
#include <utility>

using namespace mu;

struct Tst {
  int  x = 1;
  int  y = 2;
  int  z = 3;
  bool b = true;

  auto clone() const -> Tst { return Tst{*this}; }
};

int main(void) {
  mem::CAllocator allocator{};

  // Single object
  {
    auto val = UniquePtr<Tst>::create(&allocator);
    assert(val->x == 1);
    assert(val->y == 2);
    assert(val->z == 3);
    assert(val->b == true);
    assert((*val).x == 1);
    assert((*val).y == 2);
    assert((*val).z == 3);
    assert((*val).b == true);
    const Tst* got = val.get();
    assert(got->x == 1);
    assert(got->y == 2);
    assert(got->z == 3);
    assert(got->b == true);

    auto cloned = val.clone();
    val->x      = 2;
    assert(cloned->x == 1);

    auto val2 = std::move(val);
    assert(val.valid() == false);
    assert(val2.valid() == true);
    assert(val2->x == 2);
    assert(val2->y == 2);
    assert(val2->z == 3);
    assert(val2->b == true);

    Tst* released = val2.release();
    assert(val2.valid() == false);
    assert(released->x == 2);
    assert(released->y == 2);
    assert(released->z == 3);
    assert(released->b == true);
    allocator.destroy(released);
  }

  // Slice of objects
  {
    auto val = UniquePtr<Slice<Tst>>::create(&allocator, 2);
    assert(val[0].x == 1);
    assert(val[0].y == 2);
    assert(val[0].z == 3);
    assert(val[0].b == true);
    assert(val[1].x == 1);
    assert(val[1].y == 2);
    assert(val[1].z == 3);
    assert(val[1].b == true);
    try {
      assert(val[2].x == 1);
      assert(val[2].y == 2);
      assert(val[2].z == 3);
      assert(val[2].b == true);
    } catch (common::IndexOutOfBounds& e) {
      io::Stdout().format("%s (index: %zu, lenght: %zu)", e.what(), e.idx,
                          e.len);
    }

    auto cloned = val.clone();
    val[0].x    = 2;
    assert(cloned[0].x == 1);

    auto val2 = std::move(val);
    assert(val.valid() == false);
    assert(val2.valid() == true);
    assert(val2[0].x == 2);
    assert(val2[0].y == 2);
    assert(val2[0].z == 3);
    assert(val2[0].b == true);

    Slice<Tst> released = val2.release();
    assert(val2.valid() == false);
    assert(released[0].x == 2);
    assert(released[0].y == 2);
    assert(released[0].z == 3);
    assert(released[0].b == true);
    allocator.free(released);
  }

  return 0;
}
