#pragma once

#include <tuple>
#include <type_traits>

namespace hedgedev::csl::ut::expr
{
    ///
    /// Gets the type of an element inside a parameter pack.
    ///
    /// \tparam index  The index of the element.
    /// \tparam T_args The parameter pack.
    ///
    template <size_t index, typename... T_args>
    using get_pack_type_t = std::tuple_element_t<index, std::tuple<T_args...>>;

    ///
    /// Checks if an enum instance has a bit flag set.
    ///
    /// \tparam T The enum type.
    ///
    /// \param in_mask The enum instance.
    /// \param in_flag The flag to check.
    ///
    template <class T, typename = std::enable_if_t<std::is_enum_v<T>>>
    inline constexpr bool has_flag(T in_mask, T in_flag);

    ///
    /// Checks if all of the specified template types are the same type.
    ///
    /// \tparam T_first The first type.
    /// \tparam T_args  The remaining types.
    ///
    /// \returns `true` if all of the specified template types are the same.
    ///          Otherwise, `false`.
    ///
    template <typename T_first, typename... T_args>
    inline constexpr bool is_all_same_v = std::conjunction_v<std::is_same<std::remove_cvref_t<std::decay_t<T_first>>, std::remove_cvref_t<std::decay_t<T_args>>>...>;
}

#include "expr.inl"
#include "string_types.h"
