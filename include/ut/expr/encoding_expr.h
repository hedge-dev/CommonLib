#pragma once

#include <type_traits>

#include "ut/encoding_type.h"

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
    /// Gets the character size of the encoding type.
    ///
    inline constexpr size_t get_encoding_char_size(encoding::encoding_type in_encoding);

    ///
    /// Gets an encoding type suitable for the string type.
    ///
    template <typename T>
    inline constexpr encoding::encoding_type get_char_encoding_type();

    ///
    /// Gets the size of the byte order mark of a specific encoding type.
    ///
    template <encoding::encoding_type encoding>
    inline constexpr size_t get_bom_size();

    ///
    /// An `std::basic_string` inferred from an encoding type.
    ///
    template <encoding::encoding_type encoding>
    using encoded_string_t = std::basic_string<get_encoding_char_type_t<encoding>>;
}

#include "encoding_expr.inl"
