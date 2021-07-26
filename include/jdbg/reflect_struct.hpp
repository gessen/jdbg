#pragma once

#include <jdbg/detail/map_macro.hpp>
#include <jdbg/detail/meta.hpp>

#include <type_traits>
#include <utility>

namespace jdbg {

/// Utility class used to inspect structs and classes
///
/// \note Functionality of this class depends on how the macros
///       JDBG_REFLECT_STRUCT and JDBG_REFLECT_STRUCT_DERIVED were used
template <typename T>
struct struct_reflector {
  /// Type of struct or class
  using struct_type = T;

  /// List of parent structs and classes
  using base_types = detail::type_pack<>;

  /// Indicates whether struct_reflector can be used for the object type
  static constexpr bool is_reflectable = false;

  /// Returns the number of members.
  static constexpr int count()
  {
    static_assert(detail::always_false<struct_type>::value,
                  "struct was not reflected");
    return 0;
  }

  /// Returns the total number of members including derived ones
  static constexpr int total_count()
  {
    static_assert(detail::always_false<struct_type>::value,
                  "struct was not reflected");
    return 0;
  }

  /// Returns the name of the object
  static constexpr const char* name()
  {
    static_assert(detail::always_false<struct_type>::value,
                  "struct was not reflected");
    return "";
  }

  /// Applies visitor to object
  template <typename Visitor, typename S>
  static constexpr void apply(Visitor&& /*vis*/, S&& /*s*/)
  {
    static_assert(detail::always_false<struct_type>::value,
                  "struct was not reflected");
  }

  /// Applies visitor to two structs/classes of the same type
  template <typename Visitor, typename S1, typename S2>
  static constexpr void apply(Visitor&& /*vis*/, S1&& /*s1*/, S2&& /*s2*/)
  {
    static_assert(detail::always_false<struct_type>::value,
                  "struct was not reflected");
  }
};

/// Indicates whether struct_reflector can be used for the specific struct type
template <typename T>
using is_struct_reflectable = std::integral_constant<
    bool, struct_reflector<detail::remove_cvref_t<T>>::is_reflectable>;

/// Creates a struct_reflector of a given type
template <typename T>
constexpr auto struct_reflect() -> struct_reflector<T>
{
  static_assert(is_struct_reflectable<T>::value, "struct was not reflected");
  return {};
}

/// Applies visitor to a struct
///
/// \param s Visited struct
/// \param vis Visitor that accepts a field name and its reference
template <typename Visitor, typename S>
void visit_struct(S&& s, Visitor&& vis)
{
  struct_reflector<detail::remove_cvref_t<S>>::apply(std::forward<Visitor>(vis),
                                                     std::forward<S>(s));
}

/// Applies visitor to two structs that were reflected in the same way
///
/// \param s1 Visited struct
/// \param s2 Visited struct
/// \param vis Visitor that accepts both fields name and their references
template <typename Visitor, typename S1, typename S2>
void visit_structs(S1&& s1, S2&& s2, Visitor&& vis)
{
  struct_reflector<detail::remove_cvref_t<S1>>::apply(
      std::forward<Visitor>(vis), std::forward<S1>(s1), std::forward<S2>(s2));
}

} // namespace jdbg

/// \def JDBG_REFLECT_STRUCT(struct_name, ...)
/// Defines template specialisation for struct_reflector
///
/// This macro defines jdbg::struct_reflector<T> class with the following
/// members:
///   - jdbg::struct_reflector<T>::count(): returns a number of defined struct
///     fields
///   - jdbg::struct_reflector<T>::total_count(): returns a number of defined
///     struct fields including derived ones
///   - jdbg::struct_reflector<T>::name(): returns a string with a fully
///     qualified name of struct type
///   - jdbg::struct_reflector<T>::apply(): applies a visitor to struct(s)
///
/// \note This macro must be used in global scope
///
/// Parameters:
///   - Fully qualified struct type name
///   - List of struct fields
///
/// Example usage:
/// \code
///   namespace ns::detail {
///   struct my_struct {
///       int i;
///       double d;
///       std::string str;
///   };
///   }
///
///   JDBG_REFLECT_STRUCT(ns::detail::my_type, i, d, str);
///   ns::detail::my_struct ms{1, 2.5, "qwe"};
///   jdbg::visit_struct(ms, [](std::string_view name, const auto& value) {
///       std::cout << name << ": " << value << '\n';
///   });
/// \endcode
#define JDBG_REFLECT_STRUCT(struct_name, ...)                                  \
  template <>                                                                  \
  struct JDBG_REFLECT_STRUCT_must_be_defined_in_global_scope<>;                \
  JDBG_REFLECT_STRUCT_IMPL(struct_name, (), __VA_ARGS__)                       \
  static_assert(true, "force trailing semicolon")

/// \def JDBG_REFLECT_STRUCT_DERIVED(...)
/// Defines template specialisation for struct_reflector that includes derived
/// members
///
/// This macro defines jdbg::struct_reflector<T> class with the following
/// members:
///   - jdbg::struct_reflector<T>::count(): returns a number of defined struct
///     fields
///   - jdbg::struct_reflector<T>::total_count(): returns a number of defined
///     struct fields including derived ones
///   - jdbg::struct_reflector<T>::name(): returns a string with a fully
///     qualified name of struct type
///   - jdbg::struct_reflector<T>::apply(): applies a visitor to struct
///
/// \note This macro must be used in global scope
///
/// Parameters:
///   - Fully qualified struct type name
///   - Tuple of parent structs/classes. If parent structs and classes do not
///     have struct_reflector defined for them, it treats them as if they have 0
///     members
///   - List of struct fields
///
/// Example usage:
/// \code
///   namespace ns::detail {
///   struct my_struct {
///       int i;
///       double d;
///       std::string str;
///   };
///   }
///
///   struct my_struct : ns::detail::my_struct {
///       std::optional<std::string>> ostr;
///   };
///
///   JDBG_REFLECT_STRUCT(ns::detail::my_struct, i, d, str);
///   JDBG_REFLECT_STRUCT_DERIVED(my_struct, (ns::detail::my_struct), ostr);
///
///   my_struct ms{};
///   ms.i = 1;
///   ms.d = 2.5;
///   ms.str = "qwe";
///   ms.ostr = "asd";
///
///   jdbg::visit_struct(ms, [](std::string_view name, const auto& value) {
///       std::cout << name << ": " << value << '\n';
///   });
/// \endcode
#define JDBG_REFLECT_STRUCT_DERIVED(struct_name, bases, ...)                   \
  template <>                                                                  \
  struct JDBG_REFLECT_STRUCT_must_be_defined_in_global_scope<>;                \
  JDBG_REFLECT_STRUCT_IMPL(                                                    \
      struct_name, (JDBG_REFLECTION_PP_REMOVE_PARENS(bases)), __VA_ARGS__)     \
  static_assert(true, "force trailing semicolon")

#define JDBG_REFLECT_STRUCT_FIELD_COUNT(member_name) +1

#define JDBG_REFLECT_STRUCT_APPLY(member_name)                                 \
  std::forward<Visitor>(vis)(JDBG_REFLECTION_PP_STRINGIZE(member_name),        \
                             std::forward<S>(s).member_name);

#define JDBG_REFLECT_STRUCT_APPLY2(member_name)                                \
  std::forward<Visitor>(vis)(JDBG_REFLECTION_PP_STRINGIZE(member_name),        \
                             std::forward<S1>(s1).member_name,                 \
                             std::forward<S2>(s2).member_name);

#define JDBG_REFLECT_STRUCT_IMPL(struct_name, bases, ...)                      \
  namespace jdbg {                                                             \
  template <>                                                                  \
  struct struct_reflector<struct_name> {                                       \
    using struct_type = struct_name;                                           \
                                                                               \
    using base_types =                                                         \
        detail::type_pack<JDBG_REFLECTION_PP_REMOVE_PARENS(bases)>;            \
                                                                               \
    static constexpr bool is_reflectable = true;                               \
                                                                               \
    static constexpr int count()                                               \
    {                                                                          \
      return 0 JDBG_REFLECTION_PP_MAP(JDBG_REFLECT_STRUCT_FIELD_COUNT,         \
                                      __VA_ARGS__);                            \
    }                                                                          \
                                                                               \
    static constexpr int total_count()                                         \
    {                                                                          \
      return count() + detail::bases_count(base_types{});                      \
    }                                                                          \
                                                                               \
    static constexpr const char* name()                                        \
    {                                                                          \
      return JDBG_REFLECTION_PP_STRINGIZE(struct_name);                        \
    }                                                                          \
                                                                               \
    template <typename Visitor, typename S>                                    \
    static void apply(Visitor&& vis, S&& s)                                    \
    {                                                                          \
      detail::bases_apply(std::forward<Visitor>(vis), std::forward<S>(s),      \
                          base_types{});                                       \
      JDBG_REFLECTION_PP_MAP(JDBG_REFLECT_STRUCT_APPLY, __VA_ARGS__)           \
    }                                                                          \
                                                                               \
    template <typename Visitor, typename S1, typename S2>                      \
    static void apply(Visitor&& vis, S1&& s1, S2&& s2)                         \
    {                                                                          \
      detail::bases_apply(std::forward<Visitor>(vis), std::forward<S1>(s1),    \
                          std::forward<S2>(s2), base_types{});                 \
      JDBG_REFLECTION_PP_MAP(JDBG_REFLECT_STRUCT_APPLY2, __VA_ARGS__)          \
    }                                                                          \
  };                                                                           \
  static_assert(struct_reflector<struct_name>::count() <=                      \
                    max_visitable_members,                                     \
                "too many visitable members");                                 \
  }

namespace jdbg {
namespace detail {

template <typename Visitor, typename S, typename... Ts>
constexpr void bases_apply(Visitor&& vis, S&& s, type_pack<Ts...> /*pack*/)
{
  using swallow = int[];
  (void)swallow{0, ((void)(struct_reflector<Ts>::apply(
                        std::forward<Visitor>(vis), std::forward<S>(s))),
                    0)...};
}

template <typename Visitor, typename S1, typename S2, typename... Ts>
constexpr void bases_apply(Visitor&& vis, S1&& s1, S2&& s2,
                           type_pack<Ts...> /*pack*/)
{
  using swallow = int[];
  (void)swallow{0, ((void)(struct_reflector<Ts>::apply(
                        std::forward<Visitor>(vis), std::forward<S1>(s1),
                        std::forward<S2>(s2))),
                    0)...};
}

template <typename... Ts>
constexpr int bases_count(type_pack<Ts...> /*pack*/)
{
  int count = 0;
  using swallow = int[];
  (void)swallow{0,
                ((void)(count += struct_reflector<Ts>::total_count()), 0)...};
  return count;
}

} // namespace detail
} // namespace jdbg

template <typename...>
struct JDBG_REFLECT_STRUCT_must_be_defined_in_global_scope;
