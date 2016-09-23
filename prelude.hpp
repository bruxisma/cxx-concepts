#ifndef PRESENTATION_PRELUDE_HPP
#define PRESENTATION_PRELUDE_HPP

#include <type_traits>
#include <utility>

using std::is_copy_constructible;
using std::is_copy_assignable;
using std::is_destructible;
using std::is_convertible;
using std::is_pointer;
using std::is_same;

using std::integral_constant;
using std::false_type;
using std::true_type;

using std::declval;

using std::add_pointer_t;
using std::enable_if_t;

template <bool B> using bool_constant = integral_constant<bool, B>;

template <class...> using void_t = void;
template <class T> struct identity { using type = T; };

template <class T, class Void, template <class...> class, class...>
struct detector : identity<T> { using value_t = false_type; };

template <class T, template <class...> class U, class... Args>
struct detector<T, void_t<U<Args...>>, U, Args...> :
  identity<U<Args...>>
{ using value_t = true_type; };

struct nonesuch final {
  nonesuch (nonesuch const&) = delete;
  nonesuch () = delete;
  ~nonesuch () = delete;

  void operator = (nonesuch const&) = delete;
};

template <class T, template <class...> class U, class... Args>
using detected_or = detector<T, void, U, Args...>;

template <template <class...> class T, class... Args>
using detected_t = typename detected_or<nonesuch, T, Args...>::type;

template <class T, template <class...> class U, class... Args>
using detected_or_t = typename detected_or<T, U, Args...>::type;

template <class To, template <class...> class T, class... Args>
using is_detected_convertible = is_convertible<
  detected_t<T, Args...>,
  To
>;

template <class T, template <class...> class U, class... Args>
using is_detected_exact = is_same<T, detected_t<U, Args...>>;

template <template <class...> class T, class... Args>
using is_detected = typename detected_or<nonesuch, T, Args...>::value_t;

template <class...> struct conjunction;
template <class...> struct disjunction;
template <class B> using negation = bool_constant<not B::value>;

template <class T, class... Ts>
struct conjunction<T, Ts...> :
  bool_constant<T::value and conjunction<Ts...>::value>
{ };
template <> struct conjunction<> : true_type { };

template <class T, class... Ts>
struct disjunction<T, Ts...> :
  bool_constant<T::value or disjunction<Ts...>::value>
{ };

template <> struct disjunction<> : false_type { };

template <class From, class To>
using explicit_cast = decltype(static_cast<To>(declval<From>()));

#endif /* PRESENTATION_PRELUDE_HPP */
