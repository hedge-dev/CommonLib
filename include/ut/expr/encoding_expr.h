#pragma once

#include <type_traits>

#include "ut/encoding_type.h"
#include "string_expr.h"

namespace hedgedev::csl::ut::expr
{
    template <encoding::encoding_type encoding>
    struct get_encoding_char_type { using type = char; };

    template <>
    struct get_encoding_char_type<encoding::utf8> { using type = char8_t; };

    template <>
    struct get_encoding_char_type<encoding::utf16_le> { using type = char16_t; };

    template <>
    struct get_encoding_char_type<encoding::utf16_be> { using type = char16_t; };

    template <>
    struct get_encoding_char_type<encoding::utf32_le> { using type = char32_t; };

    template <>
    struct get_encoding_char_type<encoding::utf32_be> { using type = char32_t; };

    ///
    /// Gets a character type suitable for the encoding.
    ///
    /// \returns `char8_t` for UTF-8, `char16_t` for UTF-16 or `char32_t` for UTF-32.
    ///          If unknown, `char`.
    ///
    template <encoding::encoding_type encoding>
    using get_encoding_char_type_t = typename get_encoding_char_type<encoding>::type;

    ///
    /// Gets an encoding type suitable for the character type.
    ///
    template <any_string T>
    inline constexpr encoding::encoding_type get_char_encoding_type();

    ///
    /// Gets the native endianness of the specified encoding type.
    ///
    inline constexpr encoding::encoding_type get_native_encoding_type(encoding::encoding_type in_encoding);

    ///
    /// Gets the character size of the encoding type.
    ///
    inline constexpr size_t get_encoding_char_size(encoding::encoding_type in_encoding);

    ///
    /// Checks if the encoding type matches the string type.
    ///
    /// \tparam T The string type.
    ///
    /// \param in_encoding The encoding type.
    ///
    /// \returns `true` if the encoding type matches the string. Otherwise, `false`.
    ///
    template <any_string T>
    inline constexpr bool is_char_encoding_type(encoding::encoding_type in_encoding);

    ///
    /// Checks if the encoding type matches the string type.
    ///
    /// \tparam T        The string type.
    /// \tparam encoding The encoding type.
    ///
    /// \returns `true` if the encoding type matches the string. Otherwise, `false`.
    ///
    template <any_string T, encoding::encoding_type encoding>
    inline constexpr bool is_char_encoding_type_v = is_char_encoding_type<T>(encoding);

    ///
    /// An `std::basic_string` inferred from an encoding type.
    ///
    template <encoding::encoding_type encoding>
    using encoded_string_t = std::basic_string<get_encoding_char_type_t<encoding>>;

    ///
    /// An `std::basic_string_view` inferred from an encoding type.
    ///
    template <encoding::encoding_type encoding>
    using encoded_string_view_t = std::basic_string_view<get_encoding_char_type_t<encoding>>;
}

#include "encoding_expr.inl"
