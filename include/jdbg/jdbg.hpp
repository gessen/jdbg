#pragma once

#include <cstddef>
#include <iomanip>
#include <ios>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

#if __cplusplus >= 201703L
#include <optional>
#include <string_view>
#include <variant>
#endif

#include <unistd.h>

namespace jdbg {

#ifndef JDBG_LOG_FUNCTION
#define JDBG_LOG_FUNCTION(str) std::cerr << (str) << '\n'
#endif

#ifndef JDBG_IS_OUTPUT_COLOURED
#define JDBG_IS_OUTPUT_COLOURED (isatty(fileno(stderr)) != 0)
#endif

namespace detail {

// clang-format off
#if defined(__clang__)
#define JDBG_PRETTY_FUNCTION __PRETTY_FUNCTION__
static constexpr std::size_t prefix_len = sizeof("const char *jdbg::detail::pretty_function() [T = ") - 1;
static constexpr std::size_t suffix_len = sizeof("]") - 1;
#elif defined(__GNUC__) && !defined(__clang__)
#define JDBG_PRETTY_FUNCTION __PRETTY_FUNCTION__
static constexpr std::size_t prefix_len = sizeof("const char *jdbg::detail::pretty_function() [with T = ") - 1;
static constexpr std::size_t suffix_len = sizeof("]") - 1;
#else
#error "Currently unsupported compiler"
#endif
// clang-format on

template <typename T>
const char* pretty_function()
{
  return JDBG_PRETTY_FUNCTION;
}

template <typename T>
std::string get_type_name()
{
  std::string type = pretty_function<T>();
  type = type.substr(prefix_len, type.size() - prefix_len - suffix_len);
  type = type.substr(0, type.find_first_of(';'));
  return type;
}

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
using is_detected =
    typename detail::detector<nonesuch, void, Op, Args...>::value_type;

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
  return detail::get_type_name<T>();
}

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

template <typename T, std::enable_if_t<!detail::is_container<T>::value &&
                                           !std::is_enum<T>::value,
                                       int> = 0>
void pretty_print(std::ostream& os, const T& val);

void pretty_print(std::ostream& os, const bool& val);

void pretty_print(std::ostream& os, const char& val);

template <size_t N>
void pretty_print(std::ostream& os, const char (&val)[N]);

void pretty_print(std::ostream& os, const char* const& val);

void pretty_print(std::ostream& os, const std::string& val);

template <typename Enum, std::enable_if_t<std::is_enum<Enum>::value, int> = 0>
void pretty_print(std::ostream& os, const Enum& val);

template <typename T1, typename T2>
void pretty_print(std::ostream& os, const std::pair<T1, T2>& val);

template <typename... Ts>
void pretty_print(std::ostream& os, const std::tuple<Ts...>& val);

template <typename Container,
          std::enable_if_t<detail::is_container<Container>::value, int> = 0>
void pretty_print(std::ostream& os, const Container& val);

#if __cplusplus >= 201703L

void pretty_print(std::ostream& os, std::string_view val);

template <typename T>
void pretty_print(std::ostream& os, const std::optional<T>& val);

template <typename... Ts>
void pretty_print(std::ostream& os, const std::variant<Ts...>& val);

#endif

template <typename Pointer>
void pretty_print(std::ostream& os, Pointer* const& val);

void pretty_print(std::ostream& os, void* const& val);

template <typename T, typename Deleter>
void pretty_print(std::ostream& os, const std::unique_ptr<T, Deleter>& val);

template <typename T>
void pretty_print(std::ostream& os, const std::shared_ptr<T>& val);

////////////////////////////////////////////////////////////////////////////////

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

template <typename T,
          std::enable_if_t<
              !detail::is_container<T>::value && !std::is_enum<T>::value, int>>
void pretty_print(std::ostream& os, const T& val)
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

template <typename Enum, std::enable_if_t<std::is_enum<Enum>::value, int>>
void pretty_print(std::ostream& os, const Enum& val)
{
  os << static_cast<std::underlying_type_t<Enum>>(val);
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

template <typename... Ts>
void pretty_print(std::ostream& os, const std::tuple<Ts...>& val)
{
  os << '(';
  detail::pretty_print_tuple(os, val, std::index_sequence_for<Ts...>{});
  os << ')';
}

template <typename Container,
          std::enable_if_t<detail::is_container<Container>::value, int>>
void pretty_print(std::ostream& os, const Container& val)
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

inline void pretty_print(std::ostream& os, std::string_view val)
{
  os << '"' << val << '"';
}

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

template <typename Pointer>
void pretty_print(std::ostream& os, Pointer* const& val)
{
  if (val == nullptr) {
    os << "nullptr";
    return;
  }
  os << val << " -> " << *val;
}

inline void pretty_print(std::ostream& os, void* const& val)
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

namespace detail {

class output {
public:
  output(const char* file, int line, const char* func, const char* expr)
      : file_{file}, line_{line}, func_{func}, expr_{expr},
        is_coloured_{JDBG_IS_OUTPUT_COLOURED}
  {
    const auto leaf_indicator = file_.find_last_of('/');
    if (leaf_indicator != std::string::npos) {
      file_ = file_.substr(leaf_indicator + 1);
    }
  }

  template <typename T>
  T&& print(const std::string& type, T&& val)
  {
    std::stringstream out;
    print_header(out);
    print_expr(out);
    print_val(out, val);
    print_type(out, type);
    JDBG_LOG_FUNCTION(out.str());

    return std::forward<T>(val);
  }

  template <int N>
  const char (&print(const std::string& /*type*/, const char (&val)[N]))[N]
  {
    // For dbg("...") usage do not print expression and type
    std::stringstream out;
    print_header(out);
    print_val(out, val);
    JDBG_LOG_FUNCTION(out.str());

    return val;
  }

private:
  void print_header(std::ostream& os)
  {
    os << ansi(ansi_faint) << '[' << file_ << ':' << line_ << " (" << func_
       << ")] " << ansi(ansi_reset);
  }

  void print_expr(std::ostream& os)
  {
    os << ansi(ansi_cyan) << expr_ << ansi(ansi_reset) << ": ";
  }

  template <typename T>
  void print_val(std::ostream& os, const T& val)
  {
    os << ansi(ansi_bold);
    pretty_print(os, val);
    os << ansi(ansi_reset);
  }

  void print_type(std::ostream& os, const std::string& type)
  {
    os << " (" << ansi(ansi_green) << type << ansi(ansi_reset) << ")";
  }

  const char* ansi(const char* code)
  {
    if (is_coloured_) {
      return code;
    }
    return ansi_empty;
  }

private:
  static constexpr const char* const ansi_empty = "";
  static constexpr const char* const ansi_bold = "\x1b[01m";
  static constexpr const char* const ansi_faint = "\x1b[02m";
  static constexpr const char* const ansi_green = "\x1b[32m";
  static constexpr const char* const ansi_cyan = "\x1b[36m";
  static constexpr const char* const ansi_reset = "\x1b[0m";

  std::string file_;
  const int line_;
  const std::string func_;
  const std::string expr_;
  const bool is_coloured_;
};

template <typename T>
T&& forward(T&& t)
{
  return std::forward<T>(t);
}

} // namespace detail
} // namespace jdbg

#ifndef JDBG_DISABLE
#define dbg(...)                                                               \
  jdbg::detail::output(__FILE__, __LINE__, __func__, #__VA_ARGS__)             \
      .print(jdbg::detail::get_type_name<decltype(__VA_ARGS__)>(),             \
             __VA_ARGS__)
#else
#define dbg(...) jdbg::detail::forward(__VA_ARGS__)
#endif

#undef JDBG_LOG_FUNCTION
#undef JDBG_PRETTY_FUNCTION
#undef JDBG_IS_OUTPUT_COLOURED
