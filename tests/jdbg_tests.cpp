#define JDBG_LOG_FUNCTION(str) std::cerr << (str)
#define JDBG_IS_OUTPUT_COLOURED (false)
#include <jdbg/jdbg.hpp>

#include <catch2/catch.hpp>

#include <ostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

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

} // namespace

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
