#pragma once

#include <tuple>
#include <type_traits>

namespace hedgedev::csl::ut::expr
{
    template <size_t Index, typename... TArgs>
    using GetPackType = std::tuple_element_t<Index, std::tuple<TArgs...>>;

    template <class T, typename = std::enable_if_t<std::is_enum_v<T>>>
    inline constexpr bool HasFlag(T in_mask, T in_flag);

    template <typename T, typename... TArgs>
    inline constexpr bool IsAllSame = std::conjunction_v<std::is_same<std::remove_cvref_t<std::decay_t<T>>, std::remove_cvref_t<std::decay_t<TArgs>>>...>;
}

#include "Expressions.inl"
#include "StringTypes.h"
