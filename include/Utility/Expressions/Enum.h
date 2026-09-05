#pragma once

#include <type_traits>

namespace hedgedev::csl::ut::expr
{
    template <class T, typename = std::enable_if_t<std::is_enum_v<T>>>
    inline constexpr bool HasFlag(T in_mask, T in_flag)
    {
        using TType = std::underlying_type_t<T>;

        return (static_cast<TType>(in_mask) & static_cast<TType>(in_flag)) == static_cast<TType>(in_flag);
    }
}
