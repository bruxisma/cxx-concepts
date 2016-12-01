#include <iterator>
#include <vector>

#include <cstdio>

#include "prelude.hpp"

using std::input_iterator_tag;

/* special names for our purposes */
template <bool... Bs>
constexpr bool require = conjunction<bool_constant<Bs>...>::value;

template <bool... Bs>
constexpr bool either = disjunction<bool_constant<Bs>...>::value;

template <bool... Bs>
constexpr bool disallow = not require<Bs...>;

template <template <class...> class Op, class... Args>
constexpr bool exists = is_detected<Op, Args...>::value;

template <class To, template <class...> class Op, class... Args>
constexpr bool casts_to = exists<explicit_cast, detected_t<Op, Args...>, To>;

template <class To, template <class...> class Op, class... Args>
constexpr bool converts_to = is_detected_convertible<To, Op, Args...>::value;

template <class Exact, template <class...> class Op, class... Args>
constexpr bool identical_to = is_detected_exact<Exact, Op, Args...>::value;

namespace adl {

using std::swap;

template <class T, class U=T>
using swap_with = decltype(swap(declval<T>(), declval<U>()));

} /* namespace adl */

/* operator detectors */
namespace ops {

template <class T, class U> using equal_to = decltype(declval<T>() == declval<U>());
template <class T, class U> using less = decltype(declval<T>() < declval<U>());
template <class T> using dereference = decltype(*declval<T>());
template <class T> using arrow = decltype(declval<T>().operator->());

template <class T> using postfix_increment = decltype(declval<T>()++);
template <class T> using prefix_increment = decltype(++declval<T>());

} /* namespace operators */

namespace alias {

template <class T> using value_type = typename T::value_type;
template <class T> using reference = typename T::reference;
template <class T> using pointer = typename T::pointer;

template <class T>
using iterator_category = typename std::iterator_traits<T>::iterator_category;

} /* namespace type */

namespace concepts {

template <class T, class U>
constexpr bool SwappableWith = exists<adl::swap_with, T, U>;

template <class T>
constexpr bool CopyConstructible = is_copy_constructible<T>::value;

template <class T> constexpr bool CopyAssignable = is_copy_assignable<T>::value;
template <class T> constexpr bool Destructible = is_destructible<T>::value;
template <class T> constexpr bool Swappable = SwappableWith<T&, T&>;
template <class T> constexpr bool Pointer = is_pointer<T>::value;

template <class T, class U=T>
constexpr bool EqualityComparable = converts_to<bool, ops::equal_to, T, U>;

template <class T>
constexpr bool Iterator = require<
  CopyConstructible<T>,
  CopyAssignable<T>,
  Destructible<T>,
  Swappable<T>,
  exists<ops::postfix_increment, T>,
  exists<ops::prefix_increment, T>,
  exists<ops::dereference, T>
>;

template <class T>
constexpr bool InputIterator = either<
  Pointer<T>,
  require<
    EqualityComparable<T>,
    Iterator<T>,
    exists<alias::value_type, T>,
    exists<alias::reference, T>,
    either<
      identical_to<detected_t<alias::reference, T>, ops::dereference, T>,
      converts_to<detected_t<alias::value_type, T>, ops::dereference, T>
    >,
    identical_to<detected_t<alias::pointer, T>, ops::arrow, T>,
    converts_to<detected_t<alias::value_type, T>, ops::dereference, T&>,
    converts_to<input_iterator_tag, alias::iterator_category, T>
  >
>;

} /* namespace concepts */

static_assert(concepts::InputIterator<std::vector<int>::iterator>, "");
static_assert(concepts::InputIterator<int*>, "");

//template <class InputIt, class UnaryFunction>
//auto each (InputIt first, InputIt last, UnaryFunction fun) {
//  static_assert(concepts::InputIterator<InputIt>, "");
//  return std::for_each(first, last, fun);
//}

/* The two definitions of 'each' below result in identical errors */
//template <class InputIt, class UnaryFunction>
//auto each (InputIt first, InputIt last, UnaryFunction fun) -> enable_if_t<
//  concepts::InputIterator<InputIt>,
//  UnaryFunction
//> {
//  for (; first != last; ++first) {
//    fun(*first);
//  }
//  return fun;
//}
//

template <
  class InputIt,
  class UnaryFunction,
  typename std::enable_if<concepts::InputIterator<InputIt>, int>::type=0
> auto each (InputIt first, InputIt last, UnaryFunction fun) {
  return std::for_each(first, last, fun);
}

int main () {
  std::vector<int> values { 1, 2, 3, 4 };
  each(values.begin(), values.end(), [] (auto&& x) { std::printf("%d", x); });
  each(1, 2, [] (auto&&) { });
  std::puts("");
}
