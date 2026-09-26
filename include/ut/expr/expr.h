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

    ///
    /// Checks if a template type supports specific template parameters.
    ///
    /// \tparam T_base The template type to check.
    ///
    template <typename, template <typename...> class T_base>
    inline constexpr bool is_supported_template_v = false;

    ///
    /// Checks if a template type supports specific template parameters.
    ///
    /// \tparam T_base The template type to check.
    /// \tparam T_args The template parameters to check.
    ///
    template <template <typename...> class T_base, typename... T_args>
    inline constexpr bool is_supported_template_v<T_base<T_args...>, T_base> = true;

    ///
    /// Gets a fundamental size type inferred from the specified size.
    ///
    /// \tparam size The size of the type to get.
    ///
    /// \remarks For unsigned types, use the `unsigned` keyword before the declaration.
    ///
    template <size_t size = sizeof(size_t), bool is_signed = false>
    requires (size <= 8)
    using inferred_size_t = std::conditional_t<size == 1,
                                std::conditional_t<is_signed, int8_t, uint8_t>,
                                std::conditional_t<(size > 1 && size <= 2),
                                    std::conditional_t<is_signed, int16_t, uint16_t>,
                                    std::conditional_t<(size > 2 && size <= 4),
                                        std::conditional_t<is_signed, int32_t, uint32_t>,
                                        std::conditional_t<(size > 4 && size <= 8),
                                            std::conditional_t<is_signed, int64_t, uint64_t>,
                                            size_t>>>>;
}

#include "expr.inl"
#include "container_expr.h"
#include "encoding_expr.h"
#include "string_expr.h"
