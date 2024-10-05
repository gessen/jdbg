#include <jdbg/jdbg.hpp>

#include <limits>
#include <map>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace {

struct my_struct {
  int i{};
  std::map<std::string, std::vector<std::string>> m;
  std::optional<char> o;
};

std::ostream& operator<<(std::ostream& os, const my_struct& ms)
{
  jdbg::pretty_print(os, ms.i);
  os << ", ";
  jdbg::pretty_print(os, ms.m);
  os << ", ";
  jdbg::pretty_print(os, ms.o);
  return os;
}

} // namespace

int main()
{
  dbg("foo");

  constexpr int x = 10;
  const int y = dbg(x * 2) + 1;

  std::vector<int> v{x, dbg(y)};
  dbg(v);

  std::map<int, double> m{
      {1, 1.1},
      {2, 2.2},
      {3, 3.3},
  };
  dbg(m);

  std::optional<std::string> str;
  dbg(str);
  str = "bar";

  const std::string_view sv = dbg(*str);
  dbg(sv);

  enum class my_enum {
    one,
    two,
    three,
  };

  std::vector<my_enum> v2{
      my_enum::one,
      my_enum::two,
      dbg(my_enum::three),
  };
  dbg(v2);

  my_struct ms;
  ms.i = std::numeric_limits<int>::max();
  ms.m.emplace("one", std::vector<std::string>{"o", "n", "e"});
  ms.m.emplace("two", std::vector<std::string>{"t", "w", "o"});
  ms.m.emplace("three", std::vector<std::string>{"t", "h", "r", "e", "e"});
  ms.o = 'x';
  dbg(ms);
}
