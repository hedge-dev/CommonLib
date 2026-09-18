#pragma once

#include <optional>
#include <string>
#include <type_traits>
#include <vector>

#include "expr/string_types.h"

namespace hedgedev::csl::ut::string
{
    ///
    /// Compares two strings.
    ///
    /// \tparam T_left  The left string type.
    /// \tparam T_right The right string type.
    ///
    /// \param in_left           The first string.
    /// \param in_right          The second string.
    /// \param in_case_sensitive Determines whether the comparison should be
    ///                          case sensitive.
    ///
    /// \returns `true` if the strings are identical. Otherwise, `false`.
    ///
    template <expr::any_string_t T_left, expr::any_string_t T_right>
    inline bool compare(const T_left& in_left, const T_right& in_right, bool in_case_sensitive = true);

    ///
    /// Checks if a string contains a substring.
    ///
    /// \tparam T_str    The input string type.
    /// \tparam T_substr The substring type.
    ///
    /// \param in_str            The string to check.
    /// \param in_substr         The substring to search for.
    /// \param in_case_sensitive Determines whether the search should be case sensitive.
    ///
    /// \returns `true` if the input string contains the specified substring.
    ///          Otherwise, `false`.
    ///
    template <expr::any_string_t T_str, expr::any_string_t T_substr>
    inline bool contains(const T_str& in_str, const T_substr& in_substr, bool in_case_sensitive = true);

    ///
    /// Converts a string to a different encoding format.
    ///
    /// \tparam T_result The string type to convert to.
    /// \tparam T_str    The string type to convert from.
    ///
    /// \param in_str The string to convert.
    ///
    /// \returns If successful, the input string in the destination format.
    ///          Otherwise, an empty string in the destination format.
    ///
    template <expr::any_string_t T_result, expr::any_string_t T_str>
    inline std::conditional_t<std::is_same_v<T_result, T_str> && !std::is_pointer_v<T_str>, const T_result&, T_result> convert(const T_str& in_str);

    ///
    /// Escapes all instances of a character in a string with an escape sequence.
    ///
    /// \tparam T_str      The input string type.
    /// \tparam T_str_char The search and escape character type inferred from the input
    ///                    string type.
    ///
    /// \param in_str         The string to escape.
    /// \param in_search_char The character to search for and escape.
    /// \param in_escape_char The character to escape with.
    ///
    /// \returns The input string where all characters matching the search character
    ///          have been escaped using the escape character.
    ///
    template <expr::any_string_t T_str, typename T_str_char = expr::get_char_type_t<T_str>>
    inline expr::inferred_string_t<T_str> escape(const T_str& in_str, T_str_char in_search_char, T_str_char in_escape_char = T_str_char('\\'));

    ///
    /// Formats a string.
    ///
    /// \tparam T_str The input string type.
    ///
    /// \param in_str The string to format.
    ///
    /// \returns The input string with the formatters filled in.
    ///
    template <expr::any_string_t T>
    inline expr::inferred_string_t<T> format(const T in_str, ...);

    ///
    /// Gets the width of a string.
    ///
    /// \tparam T_str The input string type.
    ///
    /// \param in_str The string to search.
    ///
    /// \returns The width of the string in characters determined by the longest line.
    ///
    template <expr::any_string_t T>
    inline size_t get_width(const T& in_str);

    ///
    /// Formats a string to have a HTML hyperlink.
    ///
    /// \tparam T_str    The input string type.
    /// \tparam T_url    The URL string type.
    /// \tparam T_result The output string type inferred from the precedent
    ///                  input strings.
    ///
    /// \param in_str The string to display.
    /// \param in_url The URL to use.
    ///
    /// \returns The input string formatted with a HTML hyperlink with a URL target.
    ///
    template <expr::any_string_t T_str, expr::any_string_t T_url, typename T_result = expr::pi_string_t<T_str, T_url>>
    inline T_result hyperlink(const T_str& in_str, const T_url& in_url);

    ///
    /// Checks if a string of any type is null or empty.
    ///
    /// \tparam T_str The input string type.
    ///
    /// \param in_str The string to check.
    ///
    /// \returns `true` if the string is null or empty. Otherwise, `false`.
    ///
    template <expr::any_string_t T>
    inline bool is_null_or_empty(const T& in_str);

    ///
    /// Checks if a string of any type is null, empty, or consists only of
    /// white-space characters.
    ///
    /// \tparam T_str The input string type.
    ///
    /// \param in_str The string to check.
    ///
    /// \returns `true` if the string is null or empty, or if the string consists only
    ///          of white-space characters. Otherwise, `false`.
    ///
    template <expr::any_string_t T>
    inline bool is_null_or_whitespace(const T& in_str);

    ///
    /// Joins a collection of strings together using a delimiter.
    ///
    /// \tparam T_delimiter The delimiter string type.
    /// \tparam T_strings   The collection string type.
    ///
    /// \param in_delimiter The delimiter to join the strings with.
    /// \param in_strings   The strings to join.
    ///
    /// \returns The input strings joined together, separated by the delimiter.
    ///
    template <expr::any_string_t T_delimiter, expr::any_string_t T_strings>
    inline expr::inferred_string_t<T_strings> join(const T_delimiter& in_delimiter, const std::vector<T_strings>& in_strings);

    ///
    /// Joins a collection of strings together using a delimiter.
    ///
    /// \tparam T_delimiter The delimiter string type.
    /// \tparam T_args      The string types.
    /// \tparam T_result    The output string type inferred from the precedent
    ///                     input strings.
    ///
    /// \param in_delimiter The delimiter to join the strings with.
    /// \param in_args      The strings to join.
    ///
    /// \returns The input strings joined together, separated by the delimiter.
    ///
    template <expr::any_string_t T_delimiter, expr::any_string_t... T_args, typename T_result = expr::pi_string_t<T_delimiter, T_args...>>
    inline T_result join(const T_delimiter& in_delimiter, const T_args&... in_args);

    ///
    /// Pads a string with another.
    ///
    /// \tparam T_str     The input string type.
    /// \tparam T_pad_str The pad string type.
    /// \tparam T_result  The output string type inferred from the precedent
    ///                   input strings.
    ///
    /// \param in_str     The string to pad.
    /// \param in_pad_str The string to pad with.
    ///
    /// \returns The input string padded on either side with the pad string.
    ///
    template <expr::any_string_t T_str, expr::any_string_t T_pad_str, typename T_result = expr::pi_string_t<T_str, T_pad_str>>
    inline T_result pad(const T_str& in_str, const T_pad_str& in_pad_str);

    ///
    /// Parses a value from a string.
    ///
    /// \tparam T_result The value type to parse.
    /// \tparam T_str    The input string type.
    ///
    /// \param in_str The string to parse.
    ///
    /// \returns The value that was parsed.
    ///
    template <typename T_result, expr::any_string_t T_str>
    inline T_result parse(const T_str& in_str);

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
    template <expr::any_string_t... T_args, typename T_result = expr::pi_string_t<T_args...>>
    inline std::array<T_result, sizeof...(T_args)> precedent_convert(const T_args&... in_args);

    ///
    /// Removes XML tags from a string.
    ///
    /// \tparam T_str The input string type.
    ///
    /// \param in_str The string to omit XML tags from.
    ///
    /// \returns The input string with XML tags removed.
    ///
    template <expr::any_string_t T_str>
    inline expr::inferred_string_t<T_str> remove_xml_tags(const T_str& in_str);

    ///
    /// Splits a string into a collection using a delimiter.
    ///
    /// \tparam T_str       The input string type.
    /// \tparam T_delimiter The delimiter string type.
    /// \tparam T_result    The output string type inferred from the precedent
    ///                     input strings.
    ///
    /// \param in_str       The string to split.
    /// \param in_delimiter The delimiter to split the string with.
    ///
    /// \returns A collection of strings split by the delimiter.
    ///
    template <expr::any_string_t T_str, expr::any_string_t T_delimiter, typename T_result = expr::pi_string_t<T_str, T_delimiter>>
    inline std::vector<T_result> split(const T_str& in_str, const T_delimiter& in_delimiter);

    ///
    /// Transforms a string to lowercase.
    ///
    /// \tparam T_str The input string type.
    ///
    /// \param in_str The string to transform to lowercase.
    ///
    /// \returns The input string as lowercase.
    ///
    template <expr::any_string_t T>
    inline expr::inferred_string_t<T> lower(const T& in_str);

    ///
    /// Transforms a string to uppercase.
    ///
    /// \tparam T_str The input string type.
    ///
    /// \param in_str The string to transform to uppercase.
    ///
    /// \returns The input string as uppercase.
    ///
    template <expr::any_string_t T>
    inline expr::inferred_string_t<T> upper(const T& in_str);

    ///
    /// Removes all leading occurrences of a set of characters from the input string.
    ///
    /// \tparam T_str        The input string type.
    /// \tparam T_trim_chars The trim character type inferred from the input string type.
    ///
    /// \param in_str        The string to trim.
    /// \param in_trim_chars The characters to trim off.
    ///
    /// \returns The input string with all occurrences of the specified characters
    ///          removed from the start.
    ///
    template <expr::any_string_t T_str, typename T_trim_chars = expr::get_char_type_t<T_str>>
    inline expr::inferred_string_t<T_str> trim_start(const T_str& in_str, std::optional<std::vector<T_trim_chars>> in_trim_chars = {});

    ///
    /// Removes all trailing occurrences of a set of characters from the input string.
    ///
    /// \tparam T_str        The input string type.
    /// \tparam T_trim_chars The trim character type inferred from the input string type.
    ///
    /// \param in_str        The string to trim.
    /// \param in_trim_chars The characters to trim off.
    ///
    /// \returns The input string with all occurrences of the specified characters
    ///          removed from the end.
    ///
    template <expr::any_string_t T_str, typename T_trim_chars = expr::get_char_type_t<T_str>>
    inline expr::inferred_string_t<T_str> trim_end(const T_str& in_str, std::optional<std::vector<T_trim_chars>> in_trim_chars = {});

    ///
    /// Removes all leading and trailing occurrences of a set of characters from the
    /// input string.
    ///
    /// \tparam T_str        The input string type.
    /// \tparam T_trim_chars The trim character type inferred from the input string type.
    ///
    /// \param in_str        The string to trim.
    /// \param in_trim_chars The characters to trim off.
    ///
    /// \returns The input string with all occurrences of the specified characters
    ///          removed from the start and end.
    ///
    template <expr::any_string_t T_str, typename T_trim_chars = expr::get_char_type_t<T_str>>
    inline expr::inferred_string_t<T_str> trim(const T_str& in_str, std::optional<std::vector<T_trim_chars>> in_trim_chars = {});

    ///
    /// Truncates a string.
    ///
    /// \tparam T_str The input string type.
    ///
    /// \param in_str          The string to truncate.
    /// \param in_max_length   The maximum length to truncate to.
    /// \param in_truncate_end Determines whether to truncate the end of the string,
    ///                        rather than the beginning.
    /// \param in_ellipsis     Determines whether to add an ellipsis to the truncated
    ///                        side of the string.
    ///
    /// \returns The input string truncated to the specified length.
    ///
    template <expr::any_string_t T>
    inline expr::inferred_string_t<T> truncate(const T& in_str, size_t in_max_length, bool in_truncate_end = true, bool in_ellipsis = true);

    ///
    /// Transforms a value into a hexadecimal string.
    ///
    /// \tparam T_result The output string type.
    /// \tparam T_value  The input value type.
    ///
    /// \param in_value      The value to transform.
    /// \param in_max_length The maximum number of leading zeroes to display.
    /// \param in_prefix     Determines whether to use the "0x" prefix.
    ///
    /// \returns The input value represented as a hexadecimal string.
    ///
    template <expr::basic_string_t T_result, typename T_value>
    inline T_result hex(T_value in_value, size_t in_max_length = sizeof(size_t) * 2, bool in_prefix = true);

    ///
    /// Tries to convert a string to a different encoding format.
    ///
    /// \tparam T_result The string type to convert to.
    /// \tparam T_str    The string type to convert from.
    ///
    /// \param in_str     The string to convert.
    /// \param out_result The output string to set.
    ///
    /// \returns `true` if the conversion was successful. Otherwise, `false`.
    ///
    template <expr::any_string_t T_result, expr::any_string_t T_str>
    inline bool try_convert(const T_str& in_str, T_result& out_result);

    ///
    /// Tries to parse a value from a string.
    ///
    /// \tparam T_result The value type to parse.
    /// \tparam T_str    The input string type.
    ///
    /// \param in_str     The string to parse.
    /// \param out_result The output value to set.
    ///
    /// \returns `true` if the parse was successful. Otherwise, `false`.
    ///
    template <typename T_result, expr::any_string_t T_str>
    inline bool try_parse(const T_str& in_str, T_result& out_result);

    ///
    /// Wraps a string to a fixed character length on each line.
    ///
    /// \tparam T_str The input string type.
    ///
    /// \param in_str       The string to wrap.
    /// \param in_max_width The maximum width of each line.
    ///
    /// \returns The input string with line breaks placed around word boundaries to fit
    ///          within the maximum line width.
    ///
    template <expr::any_string_t T_str>
    inline expr::inferred_string_t<T_str> wrap(const T_str& in_str, size_t in_max_width);
}

#include "string.inl"

__CMNLIB_INTERNAL_MAKE_NAMESPACE_ALIAS(hedgedev::csl::ut, string, str);
