#ifndef MU_ITERABLE
#define MU_ITERABLE

#include "mu/optional.h"
#include <concepts>
#include <type_traits>
namespace mu {

template <class T>
concept Iterable = requires(T self) {
  { self._nextImpl() } -> std::same_as<Optional<typename T::Item>>;
};

template <class Context, class Item> struct Iterator {
  auto next() -> Optional<Item>
    requires(Iterable<Context>)
  {
    Context* self = static_cast<Context*>(this);
    return self->_nextImpl();
  }

  template <typename F>
  auto forEach(F&& func) -> void
    requires requires(F&& func, Item& item) {
      requires std::invocable<F, Item&>;
      { func(item) } -> std::same_as<void>;
    }
  {
    Context*       self = static_cast<Context*>(this);
    Optional<Item> item = self->next();
    while (item.isValid()) {
      func(item.unwrap());
      item = self->next();
    }
    // for (Optional<Item>& item : self->next()) {
    //   func(item);
    // }
  }
};

} // namespace mu

#endif // !MU_ITERABLE
