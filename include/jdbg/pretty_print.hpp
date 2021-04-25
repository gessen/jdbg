#pragma once

#include <jdbg/detail/meta.hpp>

#include <iomanip>
#include <memory>
#include <ostream>
#include <string>
#include <type_traits>

#if __cplusplus >= 201703L
#include <optional>
#include <string_view>
#include <variant>
#endif

namespace jdbg {

template <typename T>
void pretty_print(std::ostream& os, const T& val, std::true_type /*true*/)
{
  os << val;
}

template <typename T>
void pretty_print(std::ostream& /*os*/, const T& /*val*/,
                  std::false_type /*false*/)
{
  static_assert(detail::has_ostream_operator<T>::value,
                "This type does not support the ostream operator<<");
}

////////////////////////////////////////////////////////////////////////////////

template <typename T>
std::enable_if_t<!detail::is_container<T>::value && !std::is_enum<T>::value,
                 void>
pretty_print(std::ostream& os, const T& val);

inline void pretty_print(std::ostream& os, const bool& val);

inline void pretty_print(std::ostream& os, const char& val);

template <size_t N>
void pretty_print(std::ostream& os, const char (&val)[N]);

inline void pretty_print(std::ostream& os, const char* const& val);

inline void pretty_print(std::ostream& os, const std::string& val);

#if __cplusplus >= 201703L
inline void pretty_print(std::ostream& os, std::string_view val);
#endif

template <typename P>
void pretty_print(std::ostream& os, P* const& val);

inline void pretty_print(std::ostream& os, void* const& val);

inline void pretty_print(std::ostream& os, const void* const& val);

template <typename T, typename Deleter>
void pretty_print(std::ostream& os, const std::unique_ptr<T, Deleter>& val);

template <typename T>
void pretty_print(std::ostream& os, const std::shared_ptr<T>& val);

template <typename T1, typename T2>
void pretty_print(std::ostream& os, const std::pair<T1, T2>& val);

template <typename... Ts>
void pretty_print(std::ostream& os, const std::tuple<Ts...>& val);

template <typename E>
std::enable_if_t<std::is_enum<E>::value, void> pretty_print(std::ostream& os,
                                                            const E& val);

template <typename Container>
std::enable_if_t<detail::is_container<Container>::value, void>
pretty_print(std::ostream& os, const Container& val);

#if __cplusplus >= 201703L
template <typename T>
void pretty_print(std::ostream& os, const std::optional<T>& val);

template <typename... Ts>
void pretty_print(std::ostream& os, const std::variant<Ts...>& val);
#endif

////////////////////////////////////////////////////////////////////////////////

template <typename T>
std::enable_if_t<!detail::is_container<T>::value && !std::is_enum<T>::value,
                 void>
pretty_print(std::ostream& os, const T& val)
{
  pretty_print(os, val, detail::has_ostream_operator<T>{});
}

inline void pretty_print(std::ostream& os, const bool& val)
{
  os << std::boolalpha << val;
}

inline void pretty_print(std::ostream& os, const char& val)
{
  os << "0x" << std::setw(2) << std::setfill('0') << std::hex
     << std::nouppercase << (0xFF & val);
}

template <size_t N>
void pretty_print(std::ostream& os, const char (&val)[N])
{
  os << val;
}

inline void pretty_print(std::ostream& os, const char* const& val)
{
  os << '"' << val << '"';
}

inline void pretty_print(std::ostream& os, const std::string& val)
{
  os << '"' << val << '"';
}

#if __cplusplus >= 201703L
inline void pretty_print(std::ostream& os, std::string_view val)
{
  os << '"' << val << '"';
}
#endif

template <typename P>
void pretty_print(std::ostream& os, P* const& val)
{
  if (val == nullptr) {
    os << "nullptr";
    return;
  }
  os << val << " -> ";
  pretty_print(os, *val);
}

inline void pretty_print(std::ostream& os, void* const& val)
{
  if (val == nullptr) {
    os << "nullptr";
    return;
  }
  os << val;
}

inline void pretty_print(std::ostream& os, const void* const& val)
{
  if (val == nullptr) {
    os << "nullptr";
    return;
  }
  os << val;
}

template <typename T, typename Deleter>
void pretty_print(std::ostream& os, const std::unique_ptr<T, Deleter>& val)
{
  pretty_print(os, val.get());
}

template <typename T>
void pretty_print(std::ostream& os, const std::shared_ptr<T>& val)
{
  pretty_print(os, val.get());
  os << " (refs: " << val.use_count() << ")";
}

template <typename T1, typename T2>
void pretty_print(std::ostream& os, const std::pair<T1, T2>& val)
{
  os << '(';
  pretty_print(os, val.first);
  os << ", ";
  pretty_print(os, val.second);
  os << ')';
}

namespace detail {

template <typename Tuple, std::size_t... Is>
void pretty_print_tuple(std::ostream& os, const Tuple& val,
                        std::index_sequence<Is...> /*seq*/)
{
  using swallow = int[];
  (void)swallow{0, ((void)(os << (Is == 0 ? "" : ", "),
                           pretty_print(os, std::get<Is>(val))),
                    0)...};
}

} // namespace detail

template <typename... Ts>
void pretty_print(std::ostream& os, const std::tuple<Ts...>& val)
{
  os << '(';
  detail::pretty_print_tuple(os, val, std::index_sequence_for<Ts...>{});
  os << ')';
}

template <typename E>
std::enable_if_t<std::is_enum<E>::value, void> pretty_print(std::ostream& os,
                                                            const E& val)
{
  os << static_cast<std::underlying_type_t<E>>(val);
}

template <typename Container>
std::enable_if_t<detail::is_container<Container>::value, void>
pretty_print(std::ostream& os, const Container& val)
{
  os << "[";

  using std::begin;
  using std::end;

  auto b = begin(val);
  auto e = end(val);

  if (b != e) {
    pretty_print(os, *(b++));
    for (; b != e; ++b) {
      os << ", ";
      pretty_print(os, *b);
    }
  } else {
    os << "<empty>";
  }

  os << "]";
}

#if __cplusplus >= 201703L
template <typename T>
void pretty_print(std::ostream& os, const std::optional<T>& val)
{
  if (!val.has_value()) {
    os << "nullopt";
    return;
  }
  pretty_print(os, val.value());
}

template <typename... Ts>
void pretty_print(std::ostream& os, const std::variant<Ts...>& val)
{
  os << '{';
  std::visit([&](auto&& arg) { pretty_print(os, arg); }, val);
  os << '}';
}
#endif

} // namespace jdbg
