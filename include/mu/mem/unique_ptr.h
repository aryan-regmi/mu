#ifndef MU_UNIQUE_PTR_H
#define MU_UNIQUE_PTR_H

#include <memory>
namespace mu {

template <typename T> class UniquePtr {
  std::unique_ptr<T> tst;
};

} // namespace mu

#endif // !MU_UNIQUE_PTR_H
