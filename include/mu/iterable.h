#ifndef MU_ITERABLE
#define MU_ITERABLE

#include "mu/optional.h"
#include <concepts>
#include <type_traits>
namespace mu {

template <class T, class Item>
concept Iterable = requires(T self) {
  { self._nextImpl() } -> std::same_as<Optional<Item>>;
};

template <class Context, class Item> class Iterator {
  auto next() -> Optional<Item>
    requires(Iterable<Context, Item>)
  {
    Context* self = static_cast<Context*>(this);
    self->_nextImpl();
  }

  template <typename F>
  auto forEach(F&& func) -> void
    requires requires(F&& func, Item& item) {
      requires(Iterable<Context, Item>);
      requires std::invocable<F, Item&>;
      { func(item) } -> std::same_as<void>;
    }
  {
    Context* self = static_cast<Context*>(this);
    for (Optional<Item>& item : self->next()) {
      func(item);
    }
  }
};

} // namespace mu

#endif // !MU_ITERABLE
