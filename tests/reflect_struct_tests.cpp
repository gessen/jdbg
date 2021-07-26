#include <jdbg/reflect_struct.hpp>

#include <catch2/catch.hpp>

#include <sstream>
#include <string>

using namespace Catch;

namespace ns {
namespace inner {

struct my_struct {
  int i{};
  double d{};
  std::string str;
};

struct my_struct2 {
  int ii{};
  double dd{};
  std::string str2;
};

struct not_reflected {
  int i;
  double d;
  std::string str;
};

struct my_struct_derived : my_struct {
  float f{};
};

struct my_struct_derived2 : my_struct_derived, my_struct2 {
  double ddd{};
};

} // namespace inner
} // namespace ns

JDBG_REFLECT_STRUCT(ns::inner::my_struct, i, d, str);
JDBG_REFLECT_STRUCT(ns::inner::my_struct2, ii, dd, str2);

JDBG_REFLECT_STRUCT_DERIVED(ns::inner::my_struct_derived,
                            (ns::inner::my_struct), f);
JDBG_REFLECT_STRUCT_DERIVED(ns::inner::my_struct_derived2,
                            (ns::inner::my_struct_derived,
                             ns::inner::my_struct2),
                            ddd);

TEST_CASE("reflect struct")
{
  using namespace ns::inner;
  using namespace jdbg;

  SECTION("properties my_struct")
  {
    CHECK(is_struct_reflectable<my_struct>::value);
    CHECK(struct_reflector<my_struct>::count() == 3);
    CHECK(struct_reflector<my_struct>::total_count() == 3);
    CHECK_THAT(struct_reflector<my_struct>::name(),
               Equals("ns::inner::my_struct"));
  }

  SECTION("properties my_struct2")
  {
    CHECK(is_struct_reflectable<my_struct2>::value);
    CHECK(struct_reflector<my_struct2>::count() == 3);
    CHECK(struct_reflector<my_struct2>::total_count() == 3);
    CHECK_THAT(struct_reflector<my_struct2>::name(),
               Equals("ns::inner::my_struct2"));
  }

  SECTION("properties not_reflected")
  {
    CHECK_FALSE(is_struct_reflectable<not_reflected>::value);
  }

  SECTION("properties my_struct_derived")
  {
    CHECK(is_struct_reflectable<my_struct_derived>::value);
    CHECK(struct_reflector<my_struct_derived>::count() == 1);
    CHECK(struct_reflector<my_struct_derived>::total_count() == 4);
    CHECK_THAT(struct_reflector<my_struct_derived>::name(),
               Equals("ns::inner::my_struct_derived"));
  }

  SECTION("properties my_struct_derived2")
  {
    CHECK(is_struct_reflectable<my_struct_derived2>::value);
    CHECK(struct_reflector<my_struct_derived2>::count() == 1);
    CHECK(struct_reflector<my_struct_derived2>::total_count() == 8);
    CHECK_THAT(struct_reflector<my_struct_derived2>::name(),
               Equals("ns::inner::my_struct_derived2"));
  }

  SECTION("visit struct")
  {
    std::stringstream ss;
    my_struct ms{1, 2.5, "foo"};

    visit_struct(ms, [&](const char* name, const auto& value) {
      ss << name << ": " << value << '\n';
    });
    CHECK_THAT(ss.str(), Equals("i: 1\nd: 2.5\nstr: foo\n"));
  }

  SECTION("visit structs")
  {
    my_struct2 ms1{1, 2.5, "foo"};
    my_struct2 ms2{};

    visit_structs(
        ms1, ms2,
        [](const char* /*name*/, const auto& src, auto& dst) { dst = src; });
    CHECK(ms1.ii == ms2.ii);
    CHECK_THAT(ms1.dd, WithinRel(ms2.dd, 0.0001));
    CHECK_THAT(ms1.str2, Equals(ms2.str2));
  }

  SECTION("visit struct derived")
  {
    std::stringstream ss;
    my_struct_derived ms{};
    ms.i = 1;
    ms.d = 2.5;
    ms.str = "foo";
    ms.f = 4.2f;

    visit_struct(ms, [&](const char* name, const auto& value) {
      ss << name << ": " << value << '\n';
    });
    CHECK_THAT(ss.str(), Equals("i: 1\nd: 2.5\nstr: foo\nf: 4.2\n"));
  }

  SECTION("visit structs derived")
  {
    my_struct_derived2 ms1;
    ms1.i = 1;
    ms1.ii = 2;
    ms1.d = 2.5;
    ms1.dd = 5.0;
    ms1.ddd = 7.5;
    ms1.str = "foo";
    ms1.str2 = "bar";
    ms1.f = 4.2f;
    my_struct_derived2 ms2{};

    visit_structs(
        ms1, ms2,
        [](const char* /*name*/, const auto& src, auto& dst) { dst = src; });
    CHECK(ms1.i == ms2.i);
    CHECK(ms1.ii == ms2.ii);
    CHECK_THAT(ms1.d, WithinRel(ms2.d, 0.0001));
    CHECK_THAT(ms1.dd, WithinRel(ms2.dd, 0.0001));
    CHECK_THAT(ms1.ddd, WithinRel(ms2.ddd, 0.0001));
    CHECK_THAT(ms1.str, Equals(ms2.str));
    CHECK_THAT(ms1.str2, Equals(ms2.str2));
    CHECK_THAT(ms1.f, WithinRel(ms2.f, 0.0001f));
  }
}
