#pragma once

#include <cstddef>
#include <map>
#include <set>
#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#if __cplusplus >= 201703L
#include <optional>
#include <string_view>
#include <variant>
#endif

namespace jdbg {
namespace detail {

// clang-format off
#if defined(__clang__)
#define JDBG_PRETTY_FUNCTION __PRETTY_FUNCTION__
constexpr std::size_t prefix_len = sizeof("const char *jdbg::detail::pretty_function() [T = ") - 1;
constexpr std::size_t suffix_len = sizeof("]") - 1;
#elif defined(__GNUC__) && !defined(__clang__)
#define JDBG_PRETTY_FUNCTION __PRETTY_FUNCTION__
constexpr std::size_t prefix_len = sizeof("const char *jdbg::detail::pretty_function() [with T = ") - 1;
constexpr std::size_t suffix_len = sizeof("]") - 1;
#else
#error "Currently unsupported compiler"
#endif
// clang-format on

template <typename T>
const char* pretty_function()
{
  return JDBG_PRETTY_FUNCTION;
}

} // namespace detail

template <typename T>
struct type_tag {};

template <typename T>
std::string get_type_name_impl(type_tag<T> /*unused*/)
{
  std::string type = detail::pretty_function<T>();
  type = type.substr(detail::prefix_len,
                     type.size() - detail::prefix_len - detail::suffix_len);
  type = type.substr(0, type.find_first_of(';'));
  return type;
}

template <typename T>
std::string get_type_name()
{
  if (std::is_const<T>::value) {
    if (std::is_pointer<T>::value) {
      return get_type_name<std::remove_const_t<T>>() + " const";
    }
    return "const " + get_type_name<std::remove_const_t<T>>();
  }
  if (std::is_volatile<T>::value) {
    if (std::is_pointer<T>::value) {
      return get_type_name<std::remove_volatile_t<T>>() + " volatile";
    }
    return "volatile " + get_type_name<std::remove_volatile_t<T>>();
  }
  if (std::is_pointer<T>::value) {
    return get_type_name<std::remove_pointer_t<T>>() + "*";
  }
  if (std::is_lvalue_reference<T>::value) {
    return get_type_name<std::remove_reference_t<T>>() + "&";
  }
  if (std::is_rvalue_reference<T>::value) {
    return get_type_name<std::remove_reference_t<T>>() + "&&";
  }
  return get_type_name_impl(type_tag<T>{});
}

inline std::string get_type_name_impl(type_tag<short>)
{
  return "short";
}

inline std::string get_type_name_impl(type_tag<unsigned short>)
{
  return "unsigned short";
}

inline std::string get_type_name_impl(type_tag<long>)
{
  return "long";
}

inline std::string get_type_name_impl(type_tag<unsigned long>)
{
  return "unsigned long";
}

inline std::string get_type_name_impl(type_tag<std::string> /*unused*/)
{
  return "std::string";
}

#if __cplusplus >= 201703L
inline std::string get_type_name_impl(type_tag<std::string_view> /*unused*/)
{
  return "std::string_view";
}
#endif

template <typename T>
std::string
    get_type_name_impl(type_tag<std::vector<T, std::allocator<T>>> /*unused*/)
{
  return "std::vector<" + get_type_name<T>() + ">";
}

template <typename K, typename V>
std::string get_type_name_impl(type_tag<std::map<K, V>> /*unused*/)
{
  return "std::map<" + get_type_name<K>() + ", " + get_type_name<V>() + ">";
}

template <typename T>
std::string get_type_name_impl(type_tag<std::set<T>> /*unused*/)
{
  return "std::set<" + get_type_name<T>() + ">";
}

template <typename K, typename V>
std::string get_type_name_impl(type_tag<std::unordered_map<K, V>> /*unused*/)
{
  return "std::unordered_map<" + get_type_name<K>() + ", " +
         get_type_name<V>() + ">";
}

template <typename T>
std::string get_type_name_impl(type_tag<std::unordered_set<T>> /*unused*/)
{
  return "std::unordered_set<" + get_type_name<T>() + ">";
}

template <typename T1, typename T2>
std::string get_type_name_impl(type_tag<std::pair<T1, T2>> /*unused*/)
{
  return "std::pair<" + get_type_name<T1>() + ", " + get_type_name<T2>() + ">";
}

namespace detail {

template <typename... Ts, std::size_t... Is>
std::string get_type_list_name_impl(std::index_sequence<Is...> /*seq*/)
{
  std::string result;
  using swallow = int[];
  (void)swallow{0, ((void)(result += (Is == 0 ? "" : ", "),
                           result += get_type_name<Ts>(), 0),
                    0)...};
  return result;
}

} // namespace detail

template <typename... Ts>
std::string get_type_name_impl(type_tag<std::tuple<Ts...>> /*unused*/)
{
  return "std::tuple<" +
         detail::get_type_list_name_impl<Ts...>(
             std::index_sequence_for<Ts...>{}) +
         ">";
}

#if __cplusplus >= 201703L
template <typename T>
inline std::string get_type_name_impl(type_tag<std::optional<T>> /*unused*/)
{
  return "std::optional<" + get_type_name<T>() + ">";
}

template <typename... Ts>
std::string get_type_name_impl(type_tag<std::variant<Ts...>> /*unused*/)
{
  return "std::variant<" +
         detail::get_type_list_name_impl<Ts...>(
             std::index_sequence_for<Ts...>{}) +
         ">";
}
#endif

} // namespace jdbg

#undef JDBG_PRETTY_FUNCTION
