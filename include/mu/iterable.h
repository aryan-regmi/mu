#ifndef MU_ITERABLE
#define MU_ITERABLE

#include "mu/optional.h"   // Optional
#include "mu/primitives.h" // usize
#include <concepts>        // same_as
#include <tuple>

namespace mu {

template <class T>
concept Iterable = requires(T self) {
  { self._nextImpl() } -> std::same_as<Optional<typename T::Item>>;
};

template <class Context, class Item> struct Iterator;

template <class Context, class ItemType>
class Enumerator : public Iterator<Enumerator<Context, ItemType>,
                                   std::tuple<usize, Optional<ItemType>>> {
public:
  using Item = std::tuple<usize, Optional<ItemType>>;

  explicit Enumerator(Context* iter)
    requires(Iterable<Context>)
      : it{iter} {}

  auto _nextImpl() -> Optional<Item> {
    Optional<ItemType> item = this->it->next();
    if (item.isValid()) {
      this->count += 1;
      return Optional<Item>(std::make_tuple(this->count - 1, item));
    }
    return Optional<Item>();
  }

private:
  Context* it;
  usize    count = 0;
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
      { Iterable<Context> };
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
  }

  auto enumerate() -> Enumerator<Context, Item> {
    Context* self = static_cast<Context*>(this);
    return Enumerator<Context, Item>(self);
  }
};

} // namespace mu

#endif // !MU_ITERABLE
