#include <jdbg/type_name.hpp>

#include <catch2/catch.hpp>

#include <map>
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

struct my_struct {};

enum class my_enum {};

template <typename T, std::size_t N>
struct my_container {};

} // namespace

TEST_CASE("get type name")
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
    CHECK_THAT(get_type_name<unsigned int>(), Equals("unsigned int"));
    CHECK_THAT(get_type_name<short>(), Equals("short"));
    CHECK_THAT(get_type_name<unsigned short>(), Equals("unsigned short"));
    CHECK_THAT(get_type_name<long>(), Equals("long"));
    CHECK_THAT(get_type_name<unsigned long>(), Equals("unsigned long"));
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
    CHECK_THAT(get_type_name<const volatile short* const volatile>(),
               Equals("const volatile short* volatile const"));
  }

  SECTION("references")
  {
    CHECK_THAT(get_type_name<double&>(), Equals("double&"));
    CHECK_THAT(get_type_name<const double&>(), Equals("const double&"));
    CHECK_THAT(get_type_name<const volatile int&>(),
               Equals("const volatile int&"));
  }

  SECTION("stl types")
  {
    CHECK_THAT(get_type_name<std::string>(), Equals("std::string"));
    CHECK_THAT(get_type_name<std::vector<std::string>>(),
               Equals("std::vector<std::string>"));
    CHECK_THAT(
        (get_type_name<std::map<std::string, std::vector<std::string>>>()),
        Equals("std::map<std::string, std::vector<std::string>>"));
    CHECK_THAT((get_type_name<std::pair<int, double>>()),
               Equals("std::pair<int, double>"));
    CHECK_THAT(
        (get_type_name<std::tuple<std::string, std::map<int, double>>>()),
        Equals("std::tuple<std::string, std::map<int, double>>"));
#if __cplusplus >= 201703L
    CHECK_THAT(get_type_name<std::string_view>(), Equals("std::string_view"));
    CHECK_THAT(get_type_name<std::optional<std::vector<std::string>>>(),
               Equals("std::optional<std::vector<std::string>>"));
    CHECK_THAT((get_type_name<
                   std::variant<int, std::string, std::optional<double>>>()),
               Equals("std::variant<int, std::string, std::optional<double>>"));
#endif
  }

  SECTION("user defined types")
  {
    CHECK_THAT(get_type_name<my_struct>(), EndsWith("my_struct"));
    CHECK_THAT(get_type_name<my_enum>(), EndsWith("my_enum"));
    CHECK_THAT((get_type_name<my_container<int, 1>>()),
               EndsWith("my_container<int, 1>"));
  }
}
