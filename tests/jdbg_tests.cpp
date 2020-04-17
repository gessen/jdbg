#define JDBG_LOG_FUNCTION(str) std::cerr << (str)
#define JDBG_IS_OUTPUT_COLOURED (false)
#include <jdbg/jdbg.hpp>

#include <catch2/catch.hpp>

#include <cstdint>
#include <iostream>
#include <limits>
#include <map>
#include <sstream>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#if __cplusplus >= 201703L
#include <optional>
#include <string_view>
#include <variant>
#endif

using namespace Catch;

namespace {

class scoped_cerr_redirect {
public:
  explicit scoped_cerr_redirect(std::ostream& os) : org_buf_{std::cerr.rdbuf()}
  {
    std::cerr.rdbuf(os.rdbuf());
  }

  ~scoped_cerr_redirect() noexcept
  {
    if (org_buf_ != nullptr) {
      std::cerr.rdbuf(org_buf_);
    }
  }

  scoped_cerr_redirect(const scoped_cerr_redirect&) = delete;
  scoped_cerr_redirect& operator=(const scoped_cerr_redirect&) = delete;

  scoped_cerr_redirect(scoped_cerr_redirect&& other) noexcept
      : org_buf_{std::exchange(other.org_buf_, nullptr)}
  {}

  scoped_cerr_redirect& operator=(scoped_cerr_redirect&& other) noexcept
  {
    org_buf_ = std::exchange(other.org_buf_, nullptr);
    return *this;
  }

private:
  std::streambuf* org_buf_;
};

class jdbg_tests {
public:
  jdbg_tests() : redirecter_{output} {}

protected:
  std::ostringstream output;
  std::ostringstream value;

private:
  scoped_cerr_redirect redirecter_;
};

template <typename T>
std::string pretty_print(T&& value)
{
  std::stringstream ss;
  jdbg::pretty_print(ss, std::forward<T>(value));
  return ss.str();
}

struct my_struct {
  int x;
};

std::ostream& operator<<(std::ostream& os, const my_struct& ms)
{
  os << "my_struct{" << ms.x << "}";
  return os;
}

enum class my_enum {
  e1 = 13,
  e2 = 37,
};

template <typename T, std::size_t N>
struct my_container {
  T data[N];
  const T* begin() const { return data; }
  const T* end() const { return data + N; }
};

} // namespace

TEST_CASE_METHOD(jdbg_tests, "pretty print")
{
  SECTION("primitives")
  {
    CHECK_THAT(pretty_print(true), Equals("true"));
    CHECK_THAT(pretty_print('a'), Equals("0x61"));
    CHECK_THAT(pretty_print(42U), Equals("42"));
    CHECK_THAT(pretty_print(13.37), Equals("13.37"));
    CHECK_THAT(pretty_print(static_cast<void*>(nullptr)), Equals("nullptr"));
    CHECK_THAT(pretty_print("helloworld"), Equals("\"helloworld\""));
    const char* test_str = "helloworld2";
    CHECK_THAT(pretty_print(test_str), Equals("\"helloworld2\""));
  }

  SECTION("std::string")
  {
    using namespace std::string_literals;
    CHECK_THAT(pretty_print("foo"s), Equals("\"foo\""));
  }

  SECTION("std::vector")
  {
    using namespace std::string_literals;
    CHECK_THAT(pretty_print(std::vector<int>{}), Equals("[<empty>]"));
    CHECK_THAT(pretty_print(std::vector<int>{1, 2, 3}), Equals("[1, 2, 3]"));
    CHECK_THAT(pretty_print(std::vector<std::string>{"one", "two", "three"}),
               Equals("[\"one\", \"two\", \"three\"]"));
  }

  SECTION("nested std::vector")
  {
    const std::vector<std::vector<std::vector<int>>> v{
        {
            {1, 2},
            {3},
            {4, 5},
        },
        {
            {11},
            {22, 33},
        },
    };
    CHECK_THAT(pretty_print(v),
               Equals("[[[1, 2], [3], [4, 5]], [[11], [22, 33]]]"));
  }

  SECTION("std::map")
  {
    const std::map<int, double> m{
        {1, 1.1},
        {2, 2.2},
        {3, 3.3},
    };
    CHECK_THAT(pretty_print(m), Equals("[(1, 1.1), (2, 2.2), (3, 3.3)]"));
  }

  SECTION("std::tuple")
  {
    using namespace std::string_literals;
    CHECK_THAT(pretty_print(std::tuple<>{}), Equals("()"));
    CHECK_THAT(pretty_print(std::tuple<int>{1}), Equals("(1)"));
    CHECK_THAT(pretty_print(std::tuple<int, std::string>{1, "one"}),
               Equals("(1, \"one\")"));
  }

  SECTION("std::unique_ptr")
  {
    CHECK_THAT(pretty_print(std::unique_ptr<int>{}), Equals("nullptr"));
    const auto ptr = std::make_unique<int>(42);
    value << static_cast<void*>(ptr.get());
    CHECK_THAT(pretty_print(ptr), Equals(value.str() + " -> 42"));
  }

  SECTION("std::shared_ptr")
  {
    CHECK_THAT(pretty_print(std::shared_ptr<int>{}),
               Equals("nullptr (refs: 0)"));
    const auto ptr1 = std::make_shared<int>(1);
    const auto ptr2 = ptr1;
    value << static_cast<void*>(ptr1.get());
    CHECK_THAT(pretty_print(ptr1), Equals(value.str() + " -> 1 (refs: 2)"));
  }

#if __cplusplus >= 201703L
  SECTION("std::string_view")
  {
    using namespace std::string_view_literals;
    CHECK_THAT(pretty_print("bar"sv), Equals("\"bar\""));
  }

  SECTION("std::optional")
  {
    CHECK_THAT(pretty_print(std::optional<int>{}), Equals("nullopt"));
    CHECK_THAT(pretty_print(std::optional{false}), Equals("false"));
  }

  SECTION("std::variant")
  {
    std::variant<int, double, std::string> var{"foo"};
    CHECK_THAT(pretty_print(var), Equals("{\"foo\"}"));
  }
#endif

  SECTION("user defined type")
  {
    my_struct ms{9001};
    CHECK_THAT(pretty_print(ms), Equals("my_struct{9001}"));
  }

  SECTION("user defined enum")
  {
    CHECK_THAT(pretty_print(my_enum::e2), Equals("37"));
  }

  SECTION("user defined container")
  {
    const my_container<int, 5> mc{1, 2, 3, 4, 5};
    CHECK_THAT(pretty_print(mc), Equals("[1, 2, 3, 4, 5]"));
  }
}

TEST_CASE_METHOD(jdbg_tests, "type name")
{
  using jdbg::get_type_name;
  SECTION("primitives")
  {
    CHECK_THAT(get_type_name<void>(), Equals("void"));
    CHECK_THAT(get_type_name<bool>(), Equals("bool"));
    CHECK_THAT(get_type_name<char>(), Equals("char"));
    CHECK_THAT(get_type_name<signed char>(), Equals("signed char"));
    CHECK_THAT(get_type_name<unsigned char>(), Equals("unsigned char"));
    CHECK_THAT(get_type_name<int>(), Equals("int"));
    CHECK_THAT(get_type_name<float>(), Equals("float"));
    CHECK_THAT(get_type_name<const double>(), Equals("const double"));
    CHECK_THAT(get_type_name<volatile char>(), Equals("volatile char"));
    CHECK_THAT(get_type_name<const volatile int>(),
               Equals("const volatile int"));
  }

  SECTION("pointers")
  {
    CHECK_THAT(get_type_name<double*>(), Equals("double*"));
    CHECK_THAT(get_type_name<const double*>(), Equals("const double*"));
    CHECK_THAT(get_type_name<void* const>(), Equals("void* const"));
    CHECK_THAT(get_type_name<const void* const>(), Equals("const void* const"));
    CHECK_THAT(get_type_name<const volatile int* const volatile>(),
               Equals("const volatile int* volatile const"));
  }

  SECTION("references")
  {
    CHECK_THAT(get_type_name<double&>(), Equals("double&"));
    CHECK_THAT(get_type_name<const double&>(), Equals("const double&"));
    CHECK_THAT(get_type_name<const volatile int&>(),
               Equals("const volatile int&"));
  }

  SECTION("user defined types")
  {
    CHECK_THAT(get_type_name<my_struct>(), EndsWith("my_struct"));
    CHECK_THAT(get_type_name<my_enum>(), EndsWith("my_enum"));
    CHECK_THAT((get_type_name<my_container<int, 1>>()),
               Contains("my_container<int, 1"));
  }
}

TEST_CASE_METHOD(jdbg_tests, "dbg macro")
{
  SECTION("part of equation")
  {
    int i = 13;
    constexpr int j = 37;
    const int k = dbg(++i) + j;

    CHECK(k == i + j);
    CHECK_THAT(output.str(), Contains("i:"));
    CHECK_THAT(output.str(), Contains(std::to_string(i)));
    CHECK_THAT(output.str(), Contains(jdbg::get_type_name<decltype(i)>()));
  }

  SECTION("part of container")
  {
    const std::vector<std::string> v{
        "one",
        dbg("two"),
        "three",
    };

    CHECK_THAT(v[1], Equals("two"));
    CHECK_THAT(output.str(), Contains("\"two\""));
  }
}
