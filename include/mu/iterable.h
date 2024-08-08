#ifndef MU_ITERABLE
#define MU_ITERABLE

#include "mu/cloneable.h"  // Cloneable
#include "mu/optional.h"   // Optional
#include "mu/primitives.h" // usize
#include <algorithm>
#include <concepts> // same_as
#include <functional>
#include <iostream>
#include <tuple>

namespace mu {

template <class T>
concept Iterable = requires(T self) {
  {
    self._nextImpl()
  } -> std::same_as<Optional<std::reference_wrapper<typename T::Item>>>;
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

  auto _nextImpl() -> Optional<std::reference_wrapper<Item>> {
    Optional<std::reference_wrapper<Item>> item = this->it->next();
    if (item.isValid()) {
      this->count += 1;
      return Optional<std::reference_wrapper<Item>>({this->count - 1, item});
    }
    return Optional<std::reference_wrapper<Item>>();
  }

private:
  Context* it;
  usize    count = 0;
};

template <class Context, class ItemType>
class Cloned : public Iterator<Cloned<Context, ItemType>, ItemType> {
public:
  using Item = ItemType;

  explicit Cloned(Context* iter)
    requires(Iterable<Context> && Cloneable<Item>)
      : it{iter} {}

  auto _nextImpl() -> Optional<std::reference_wrapper<Item>> {
    Optional<std::reference_wrapper<Item>> item = this->it->next();
    if (item.isValid()) {
      if constexpr (Copyable<Item>) {
        return Optional<std::reference_wrapper<Item>>{std::move(item.unwrap())};
      } else if constexpr (Cloneable<Item>) {
        return Optional<std::reference_wrapper<Item>>{
            std::move(item.unwrap().clone())};
      }
    }
    return Optional<std::reference_wrapper<Item>>();
  }

private:
  Context* it;
};

template <class Context, class Item> struct Iterator {
  auto next() -> Optional<std::reference_wrapper<Item>>
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
    Context*                               self = static_cast<Context*>(this);
    Optional<std::reference_wrapper<Item>> item = self->next();
    while (item.isValid()) {
      func(item.unwrap());
      item = self->next();
    }
  }

  auto enumerate() -> Enumerator<Context, Item>
    requires(Iterable<Context>)
  {
    Context* self = static_cast<Context*>(this);
    return Enumerator<Context, Item>(self);
  }

  auto cloned() -> Cloned<Context, Item>
    requires(Iterable<Context> && Cloneable<Item>)
  {
    Context* self = static_cast<Context*>(this);
    return Cloned<Context, Item>(self);
  }

  // TODO: Implement Container and collect()
  //
  // template<Container C> auto collect() -> C {
  //
  // }
  // auto collect()
};

} // namespace mu

#endif // !MU_ITERABLE
