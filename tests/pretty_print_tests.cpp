#include <jdbg/pretty_print.hpp>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

#include <cstddef>
#include <map>
#include <memory>
#include <optional>
#include <ostream>
#include <sstream>
#include <string>
#include <string_view>
#include <tuple>
#include <utility>
#include <variant>
#include <vector>

using namespace Catch::Matchers;

namespace {

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

enum class my_enum { // NOLINT
  e1 = 13,
  e2 = 37,
};

template <typename T, std::size_t N>
struct my_container {
  T data[N];
  const T* begin() const { return data; }
  const T* end() const { return data + N; }
  std::size_t size() const { return N; }
};

} // namespace

TEST_CASE("pretty print")
{
  SECTION("primitives")
  {
    CHECK_THAT(pretty_print(true), Equals("true"));
    CHECK_THAT(pretty_print('a'), Equals("0x61"));
    CHECK_THAT(pretty_print(42U), Equals("42"));
    CHECK_THAT(pretty_print(13.37), Equals("13.37"));
  }

  SECTION("pointers")
  {
    int test_i = 10;
    CHECK_THAT(pretty_print(static_cast<void*>(nullptr)), Equals("nullptr"));
    CHECK_THAT(pretty_print(static_cast<const void*>(nullptr)),
               Equals("nullptr"));
    CHECK_THAT(pretty_print(static_cast<void*>(&test_i)), StartsWith("0x"));
    CHECK_THAT(pretty_print(static_cast<const void*>(&test_i)),
               StartsWith("0x"));
    CHECK_THAT(pretty_print("helloworld"), Equals("\"helloworld\""));
    const char* test_str = "helloworld2";
    CHECK_THAT(pretty_print(test_str), Equals("\"helloworld2\""));
    const double test_d = 90.01;
    CHECK_THAT(pretty_print(&test_d), EndsWith("-> 90.01"));
  }

  SECTION("std::string")
  {
    using namespace std::string_literals;
    CHECK_THAT(pretty_print("foo"s), Equals("\"foo\"")); // NOLINT
  }

  SECTION("std::vector")
  {
    using namespace std::string_literals;
    CHECK_THAT(pretty_print(std::vector<int>{}), Equals("[<empty>]"));
    CHECK_THAT(pretty_print(std::vector<int>{1, 2, 3}), Equals("[1, 2, 3]"));
    CHECK_THAT(
        pretty_print(std::vector<int>{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11}),
        Equals("[1, 2, 3, 4, 5, 6, 7, 8, 9, 10, ... size: 11]"));
    CHECK_THAT(pretty_print(std::vector<std::string>{"one", "two", "three"}),
               Equals("[\"one\", \"two\", \"three\"]"));
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
    const std::map<int, std::string> m{
        {1, "1.1"},
        {2, "2.2"},
        {3, "3.3"},
    };
    CHECK_THAT(pretty_print(m),
               Equals("[(1, \"1.1\"), (2, \"2.2\"), (3, \"3.3\")]"));
  }

  SECTION("std::tuple")
  {
    using namespace std::string_literals;
    CHECK_THAT(pretty_print(std::tuple<>{}), Equals("()"));
    CHECK_THAT(pretty_print(std::tuple<int>{1}), Equals("(1)"));
    CHECK_THAT(pretty_print(std::tuple<int, std::string, std::vector<double>>{
                   1, "one", {1.1, 2.1}}),
               Equals("(1, \"one\", [1.1, 2.1])"));
  }

  SECTION("std::unique_ptr")
  {
    CHECK_THAT(pretty_print(std::unique_ptr<int>{}), Equals("nullptr"));
    CHECK_THAT(pretty_print(std::make_unique<int>(42)), EndsWith(" -> 42"));
  }

  SECTION("std::shared_ptr")
  {
    CHECK_THAT(pretty_print(std::shared_ptr<int>{}),
               Equals("nullptr (refs: 0)"));
    const auto ptr1 = std::make_shared<std::string>("qwe");
    const auto ptr2 = ptr1; // NOLINT
    CHECK_THAT(pretty_print(ptr1), EndsWith(" -> \"qwe\" (refs: 2)"));
  }

  SECTION("std::string_view")
  {
    using namespace std::string_view_literals;
    CHECK_THAT(pretty_print("bar"sv), Equals("\"bar\""));
  }

  SECTION("std::optional")
  {
    CHECK_THAT(pretty_print(std::optional<int>{}), Equals("nullopt"));
    CHECK_THAT(pretty_print(std::optional{false}), Equals("false"));
    CHECK_THAT(pretty_print(std::optional<std::string>{"asd"}),
               Equals("\"asd\""));
  }

  SECTION("std::variant")
  {
    const std::variant<int, double, std::string> var{"foo"};
    CHECK_THAT(pretty_print(var), Equals("{\"foo\"}"));
  }

  SECTION("user defined type")
  {
    const my_struct ms{9001};
    CHECK_THAT(pretty_print(ms), Equals("my_struct{9001}"));
  }

  SECTION("user defined enum")
  {
    CHECK_THAT(pretty_print(my_enum::e1), Equals("13"));
    CHECK_THAT(pretty_print(my_enum::e2), Equals("37"));
  }

  SECTION("user defined container")
  {
    const my_container<int, 5> mc{1, 2, 3, 4, 5};
    CHECK_THAT(pretty_print(mc), Equals("[1, 2, 3, 4, 5]"));
  }
}
