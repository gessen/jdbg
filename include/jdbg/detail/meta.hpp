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
using container_t =
    decltype(std::begin(std::declval<T&>()), std::end(std::declval<T&>()));

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

} // namespace detail
} // namespace jdbg
