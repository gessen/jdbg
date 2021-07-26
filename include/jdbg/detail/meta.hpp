#pragma once

#include <iosfwd>
#include <iterator>
#include <type_traits>

namespace jdbg {
namespace detail {

struct nonesuch {
  ~nonesuch() = delete;
  nonesuch(nonesuch const&) = delete;
  void operator=(nonesuch const&) = delete;
};

template <typename...>
using void_t = void;

template <typename Default, typename AlwaysVoid,
          template <typename...> class Op, typename... Args>
struct detector {
  using value_type = std::false_type;
  using type = Default;
};

template <typename Default, template <typename...> class Op, typename... Args>
struct detector<Default, void_t<Op<Args...>>, Op, Args...> {
  using value_type = std::true_type;
  using type = Op<Args...>;
};

template <template <typename...> class Op, typename... Args>
using is_detected = typename detector<nonesuch, void, Op, Args...>::value_type;

template <typename T>
constexpr auto size(const T& c) -> decltype(c.size())
{
  return c.size();
}

template <typename T, std::size_t N>
constexpr std::size_t size(const T (&/*unused*/)[N])
{
  return N;
}

template <typename T>
using container_t =
    decltype(std::begin(std::declval<T&>()), std::end(std::declval<T&>()),
             jdbg::detail::size(std::declval<T&>()));

template <typename T>
using ostream_operator_t =
    decltype(std::declval<std::ostream&>() << std::declval<T&>());

template <typename T>
struct is_container : is_detected<container_t, T> {};

template <typename Ch, typename Tr, typename Al>
struct is_container<std::basic_string<Ch, Tr, Al>> : std::false_type {};

#if __cplusplus >= 201703L

template <typename Ch, typename Tr>
struct is_container<std::basic_string_view<Ch, Tr>> : std::false_type {};

#endif

template <typename T>
struct has_ostream_operator : is_detected<ostream_operator_t, T> {};

template <typename T>
struct always_false : std::false_type {};

template <typename T>
struct remove_cvref {
  using type = std::remove_cv_t<std::remove_reference_t<T>>;
};

template <typename T>
using remove_cvref_t = typename remove_cvref<T>::type;

template <std::size_t N, typename Head, typename... Tail>
struct at_type {
  static_assert(N < 1 + sizeof...(Tail), "invalid arg index");
  using type = typename at_type<N - 1, Tail...>::type;
};

template <typename Head, typename... Tail>
struct at_type<0, Head, Tail...> {
  using type = Head;
};

template <std::size_t N, typename... Args>
using at_type_t = typename at_type<N, Args...>::type;

template <typename... Ts>
struct type_pack : std::integral_constant<std::size_t, sizeof...(Ts)> {
  template <std::size_t N>
  struct extract {
    using type = at_type_t<N, Ts...>;
  };

  template <std::size_t N>
  using extract_t = typename extract<N>::type;
};

} // namespace detail
} // namespace jdbg
