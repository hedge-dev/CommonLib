#pragma once

#include <type_traits>

#include "expr/encoding_expr.h"
#include "expr/string_expr.h"
#include "encoding_type.h"

namespace hedgedev::csl::ut::encoding
{
    ///
    /// Converts a string to a different encoding format.
    ///
    /// \tparam T_result The string type to convert to.
    /// \tparam T_str    The string type to convert from.
    ///
    /// \param in_str      The string to convert.
    /// \param in_encoding The encoding to convert to, inferred from the string type.
    ///
    /// \returns If successful, the input string in the destination format.
    ///          Otherwise, an empty string in the destination format.
    ///
    template <expr::basic_string T_result, expr::any_string T_str>
    inline std::conditional_t<std::is_same_v<T_result, T_str> && !std::is_pointer_v<T_str>, const T_result&, T_result> convert(const T_str& in_str, encoding_type in_encoding = expr::get_char_encoding_type<T_result>());

    ///
    /// Converts a string to a different encoding format.
    ///
    /// \tparam encoding The encoding to convert to.
    /// \tparam T_str    The string type to convert from.
    /// \tparam T_result The string type to convert to, inferred from the encoding type.
    ///
    /// \param in_str The string to convert.
    ///
    /// \returns If successful, the input string in the destination format.
    ///          Otherwise, an empty string in the destination format.
    ///
    template <encoding_type encoding, expr::any_string T_str, typename T_result = expr::encoded_string_t<encoding>>
    inline std::conditional_t<std::is_same_v<T_result, T_str> && !std::is_pointer_v<T_str>, const T_result&, T_result> convert(const T_str& in_str);

    ///
    /// Gets the byte order mark of an encoding type.
    ///
    /// \param in_encoding The encoding type.
    ///
    /// \returns An vector of bytes containing the byte order mark for the
    ///          specified encoding.
    ///
    inline std::vector<uint8_t> get_bom(ut::encoding::encoding_type in_encoding);

    ///
    /// Gets the size of the byte order mark of an encoding type.
    ///
    /// \param in_encoding The encoding type.
    ///
    /// \returns The size of the byte order mark.
    ///
    inline constexpr size_t get_bom_size(encoding_type in_encoding);

    ///
    /// Gets the size of the byte order mark of an encoding type.
    ///
    /// \tparam encoding The encoding type.
    ///
    /// \returns The size of the byte order mark.
    ///
    template <encoding_type encoding>
    inline constexpr size_t get_bom_size();

    ///
    /// Converts multiple strings to share the same encoding format.
    ///
    /// \tparam T_args   The string types.
    /// \tparam T_result The string type to convert to inferred from the string with the
    ///                  largest character size.
    ///
    /// \param in_args The strings to convert.
    ///
    /// \returns An array of strings in the same destination format.
    ///          If a string failed to convert, an empty string will be put in
    ///          its place.
    ///
    template <expr::any_string... T_args, typename T_result = expr::pi_string_t<T_args...>>
    inline std::array<T_result, sizeof...(T_args)> precedent_convert(const T_args&... in_args);

    ///
    /// Tries to convert a string to a different encoding format.
    ///
    /// \tparam T_result The string type to convert to.
    /// \tparam T_str    The string type to convert from.
    ///
    /// \param in_str       The string to convert.
    /// \param out_result   The output string to set.
    /// \param in_encoding  The encoding to convert to, inferred from the string type.
    ///
    /// \returns `true` if the conversion was successful. Otherwise, `false`.
    ///
    template <expr::basic_string T_result, expr::any_string T_str>
    inline bool try_convert(const T_str& in_str, T_result& out_result, encoding_type in_encoding = expr::get_char_encoding_type<T_result>());

    ///
    /// Tries to convert a string to a different encoding format.
    ///
    /// \tparam encoding The encoding to convert to.
    /// \tparam T_str    The string type to convert from.
    /// \tparam T_result The string type to convert to, inferred from the encoding type.
    ///
    /// \param in_str     The string to convert.
    /// \param out_result The output string to set.
    ///
    /// \returns `true` if the conversion was successful. Otherwise, `false`.
    ///
    template <encoding_type encoding, expr::any_string T_str, typename T_result = expr::encoded_string_t<encoding>>
    inline bool try_convert(const T_str& in_str, T_result& out_result);
}

#include "encoding.inl"

__CMNLIB_INTERNAL_MAKE_NAMESPACE_ALIAS(hedgedev::csl::ut, encoding, enc);
