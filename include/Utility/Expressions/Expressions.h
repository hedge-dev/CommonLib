#pragma once

#include <tuple>
#include <type_traits>

namespace hedgedev::csl::ut::expr
{
    ///
    /// Gets the type of an element inside a parameter pack.
    ///
    /// \tparam Index The index of the element.
    /// \tparam TArgs The parameter pack.
    ///
    template <size_t Index, typename... TArgs>
    using GetPackType = std::tuple_element_t<Index, std::tuple<TArgs...>>;

    ///
    /// Checks if an enum instance has a bit flag set.
    ///
    /// \tparam T The enum type.
    ///
    /// \param in_mask The enum instance.
    /// \param in_flag The flag to check.
    ///
    template <class T, typename = std::enable_if_t<std::is_enum_v<T>>>
    inline constexpr bool HasFlag(T in_mask, T in_flag);

    ///
    /// Checks if all of the specified template types are the same type.
    ///
    /// \tparam T     The first type.
    /// \tparam TArgs The remaining types.
    ///
    /// \returns `true` if all of the specified template types are the same.
    ///          Otherwise, `false`.
    ///
    template <typename T, typename... TArgs>
    inline constexpr bool IsAllSame = std::conjunction_v<std::is_same<std::remove_cvref_t<std::decay_t<T>>, std::remove_cvref_t<std::decay_t<TArgs>>>...>;
}

#include "Expressions.inl"
#include "StringTypes.h"
