#pragma once

#include <jdbg/pretty_print.hpp>
#include <jdbg/type_name.hpp> // NOLINT

#include <cstdio>
#include <iostream>
#include <sstream>
#include <string>
#include <utility>

#include <unistd.h>

#ifndef JDBG_LOG_FUNCTION
#define JDBG_LOG_FUNCTION(str) std::cerr << (str) << '\n'
#endif

#ifndef JDBG_IS_OUTPUT_COLOURED
#define JDBG_IS_OUTPUT_COLOURED (isatty(fileno(stderr)) != 0) // NOLINT
#endif

namespace jdbg::detail {

class output {
public:
  output(const char* file, int line, const char* func, // NOLINT
         const char* expr)
      : file_{file}, line_{line}, func_{func}, expr_{expr}

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

  const char* ansi(const char* code) const
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
  int line_;
  std::string func_;
  std::string expr_;
  bool is_coloured_{JDBG_IS_OUTPUT_COLOURED};
};

template <typename T>
T&& forward(T&& t)
{
  return std::forward<T>(t);
}

} // namespace jdbg::detail

#ifndef JDBG_DISABLE
#define dbg(...)                                                               \
  jdbg::detail::output(__FILE__, __LINE__, __func__, #__VA_ARGS__)             \
      .print(jdbg::get_type_name<decltype(__VA_ARGS__)>(), __VA_ARGS__)
#else
#define dbg(...) jdbg::detail::forward(__VA_ARGS__)
#endif

#undef JDBG_LOG_FUNCTION
#undef JDBG_IS_OUTPUT_COLOURED
