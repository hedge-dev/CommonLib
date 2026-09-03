#pragma once

#include <string>
#include <vector>

namespace hedgedev::csl::ut::string
{
    ///
    /// Compares two strings with indiscriminate encoding.
    ///
    /// \param in_a               The first string.
    /// \param in_b               The second string.
    /// \param in_isCaseSensitive Determines whether the comparison should be case sensitive.
    /// 
    /// \returns `true` if the strings are identical. Otherwise, `false`.
    ///
    template <typename TLiteralA, typename TLiteralB>
    inline bool Compare(const TLiteralA* in_a, const TLiteralB* in_b, bool in_isCaseSensitive = true);

    ///
    /// Checks if the input string contains a substring.
    ///
    /// \param in_str     The string to check.
    /// \param in_pSubStr The substring to search for.
    /// 
    /// \returns `true` if \ref in_str contains \ref in_pSubStr. Otherwise, `false`.
    ///
    template <typename TStringView, typename TLiteral = typename TStringView::value_type>
    inline bool Contains(TStringView in_str, const TLiteral* in_pSubStr);

    ///
    /// Converts a string to a different encoding format.
    /// 
    /// \tparam TDst The string type to convert to.
    /// \tparam TSrc The string type to convert from.
    ///
    /// \param in_rrStr The string to convert.
    /// 
    /// \returns If successful, the input string in the requested type.
    ///          Otherwise, an empty string in the requested type.
    ///
    template <typename TDst, typename TSrc>
    inline TDst Convert(TSrc&& in_rrStr);

    ///
    /// Creates a string inferred from a string literal at compile time.
    /// 
    /// \tparam TString  The string type to create.
    /// \tparam TLiteral The string literal type to infer from.
    ///
    /// \param in_pStr The string literal to create a string from.
    /// 
    /// \returns The input string in the requested type.
    ///
    template <typename TString, typename TLiteral>
    inline constexpr TString CreateInferredString(const TLiteral* in_pStr);

    ///
    /// Escapes all instances of a character in a string with an escape sequence.
    /// 
    /// \tparam TString     The output string type.
    /// \tparam TLiteral    The escape character type inferred from the output string type.
    /// \tparam TStringView The input string type inferred from the escape character type.
    ///
    /// \param in_str          The string to escape.
    /// \param in_charToEscape The character to search for and escape.
    /// \param in_escapeChar   The character used to escape with.
    /// 
    /// \returns The input string where all characters matching \ref in_charToEscape have been escaped using \ref in_escapeChar.
    ///
    template <typename TString, typename TLiteral = typename TString::value_type, typename TStringView = std::basic_string_view<TLiteral>>
    inline TString Escape(TStringView in_str, TLiteral in_charToEscape, TLiteral in_escapeChar = TLiteral('\\'));

    ///
    /// Formats a string.
    /// Use std::format where possible, this is for compatibility with strings returned by APIs that use old formatters.
    /// 
    /// \tparam TLiteral The input string type.
    /// \tparam TString  The output string type inferred from the input string type.
    ///
    /// \param in_pStr The string to format.
    /// 
    /// \returns The formatted string.
    ///
    template <typename TLiteral, typename TString = std::basic_string<TLiteral>>
    inline TString Format(const TLiteral* in_pStr, ...);

    ///
    /// Gets the width of a string.
    /// 
    /// \tparam TStringView The input string type.
    ///
    /// \param in_str The string to search.
    /// 
    /// \returns The width of the string in characters determined by the largest line.
    ///
    template <typename TStringView>
    inline size_t GetWidth(TStringView in_str);

    ///
    /// Formats a string to have a HTML hyperlink.
    /// 
    /// \tparam TStringView The input string type.
    /// \tparam TString     The output string type inferred from the input string type.
    ///
    /// \param in_str The string to display.
    /// \param in_url The URL to use.
    /// 
    /// \returns The input string formatted as a HTML hyperlink with a URL target.
    ///
    template <typename TStringView, typename TString = std::basic_string<typename TStringView::value_type>>
    inline TString Hyperlink(TStringView in_str, TStringView in_url);

    ///
    /// Joins a collection of strings together using a delimiter.
    /// 
    /// \tparam TLiteral The delimiter string type.
    /// \tparam TString  The string type inferred from the delimiter type.
    ///
    /// \param in_pDelimiter The delimiter to join the strings with.
    /// \param in_rStrings   The strings to join.
    /// 
    /// \returns The input strings joined together, separated by the delimiter.
    ///
    template <typename TLiteral, typename TString = std::basic_string<TLiteral>>
    inline TString Join(const TLiteral* in_pDelimiter, const std::vector<TString>& in_rStrings);

    ///
    /// Pads a string with a character.
    /// 
    /// \tparam TStringView The input string type.
    /// \tparam TLiteral    The pad character type inferred from the input string type.
    /// \tparam TString     The output string type inferred from the pad character type.
    ///
    /// \param in_str     The string to pad.
    /// \param in_padChar The character to pad with.
    /// 
    /// \returns The input string padded on either side with the pad character.
    ///
    template <typename TStringView, typename TLiteral = typename TStringView::value_type, typename TString = std::basic_string<TLiteral>>
    inline TString Pad(TStringView in_str, const TLiteral in_padChar = ' ');

    ///
    /// Omits HTML tags from a string.
    /// 
    /// \tparam TStringView The input string type.
    /// \tparam TString     The output string type inferred from the input string type.
    ///
    /// \param in_str The string to omit HTML tags from.
    /// 
    /// \returns The input string with HTML tags removed.
    ///
    template <typename TStringView, typename TString = std::basic_string<typename TStringView::value_type>>
    inline TString RemoveHtmlTags(TStringView in_str);

    ///
    /// Splits a string into a collection using a delimiter.
    /// 
    /// \tparam TStringView The input string type.
    /// \tparam TLiteral    The delimiter string type.
    /// \tparam TString     The output string type inferred from the delimiter string type.
    ///
    /// \param in_str        The string to split.
    /// \param in_pDelimiter The delimiter to split the string with.
    /// 
    /// \returns A collection of strings split by the delimiter.
    ///
    template <typename TStringView, typename TLiteral = typename TStringView::value_type, typename TString = std::basic_string<TLiteral>>
    inline std::vector<TString> Split(TStringView in_str, const TLiteral* in_pDelimiter);

    ///
    /// Transforms a string to lowercase.
    /// 
    /// \tparam TStringView The input string type.
    /// \tparam TString     The output string type inferred from the input string type.
    ///
    /// \param in_str The string to transform to lowercase.
    /// 
    /// \returns The input string as lowercase.
    ///
    template <typename TStringView, typename TString = std::basic_string<typename TStringView::value_type>>
    inline TString ToLower(TStringView in_str);

    ///
    /// Trims a character from the start of a string.
    /// 
    /// \tparam TStringView The input string type.
    /// \tparam TLiteral    The trim character type.
    /// \tparam TString     The output string type inferred from the trim character type.
    ///
    /// \param in_str      The string to trim.
    /// \param in_trimChar The character to trim off.
    /// 
    /// \returns The input string with all instances of the trim character omitted from the start.
    ///
    template <typename TStringView, typename TLiteral = typename TStringView::value_type, typename TString = std::basic_string<TLiteral>>
    inline TStringView TrimStart(TStringView in_str, const TLiteral in_trimChar = '\0');

    ///
    /// Trims a character from the end of a string.
    /// 
    /// \tparam TStringView The input string type.
    /// \tparam TLiteral    The trim character type.
    /// \tparam TString     The output string type inferred from the trim character type.
    ///
    /// \param in_str      The string to trim.
    /// \param in_trimChar The character to trim off.
    /// 
    /// \returns The input string with all instances of the trim character omitted from the end.
    ///
    template <typename TStringView, typename TLiteral = typename TStringView::value_type, typename TString = std::basic_string<TLiteral>>
    inline TStringView TrimEnd(TStringView in_str, const TLiteral in_trimChar = '\0');

    ///
    /// Trims a character from the start and end of a string.
    /// 
    /// \tparam TStringView The input string type.
    /// \tparam TLiteral    The trim character type.
    /// \tparam TString     The output string type inferred from the trim character type.
    ///
    /// \param in_str      The string to trim.
    /// \param in_trimChar The character to trim off.
    /// 
    /// \returns The input string with all instances of the trim character omitted from the start and end.
    ///
    template <typename TStringView, typename TLiteral = typename TStringView::value_type, typename TString = std::basic_string<TLiteral>>
    inline TStringView Trim(TStringView in_str, const TLiteral in_trimChar = '\0');

    ///
    /// Truncates a string.
    /// 
    /// \tparam TStringView The input string type.
    /// \tparam TString     The output string type inferred from the input string type.
    ///
    /// \param in_str         The string to truncate.
    /// \param in_maxLength   The maximum length to truncate to.
    /// \param in_truncateEnd Determines whether to truncate the end of the string, rather than the beginning.
    /// \param in_ellipsis    Determines whether to add an ellipsis to the truncated side of the string.
    /// 
    /// \returns The input string truncated to the specified length.
    ///
    template <typename TStringView, typename TString = std::basic_string<typename TStringView::value_type>>
    inline TString Truncate(TStringView in_str, size_t in_maxLength, bool in_truncateEnd = true, bool in_ellipsis = true);

    ///
    /// Transforms a value into a hexadecimal string.
    /// 
    /// \tparam TString The output string type.
    /// \tparam TValue  The input value type.
    ///
    /// \param in_value     The value to transform.
    /// \param in_maxLength The maximum number of leading zeroes to display.
    /// \param in_prefix    Determines whether to use the "0x" prefix.
    /// 
    /// \returns The input value represented as a hexadecimal string.
    ///
    template <typename TString, typename TValue>
    inline TString ToHex(TValue in_value, size_t in_maxLength = sizeof(size_t) * 2, bool in_prefix = true);

    ///
    /// Tries to convert a string to a different encoding format.
    /// 
    /// \tparam TDst The string type to convert to.
    /// \tparam TSrc The string type to convert from.
    ///
    /// \param in_rrStr   The string to convert.
    /// \param out_rValue The output string to set.
    /// 
    /// \returns `true` if the conversion was successful. Otherwise, `false`.
    ///
    template <typename TDst, typename TSrc>
    inline bool TryConvert(TSrc&& in_rrStr, TDst& out_rValue);

    ///
    /// Tries to parse a type from a string.
    /// 
    /// \tparam TResult     The type to parse.
    /// \tparam TStringView The input string type.
    ///
    /// \param in_str     The string to parse.
    /// \param out_rValue The output value to set.
    /// 
    /// \returns `true` if the parse was successful. Otherwise, `false`.
    ///
    template <typename TResult, typename TStringView>
    inline bool TryParse(TStringView in_str, TResult& out_rValue);

    ///
    /// Wraps a string to a fixed character length on each line.
    /// 
    /// \tparam TStringView The input string type.
    /// \tparam TString     The output string type inferred from the input string type.
    ///
    /// \param in_str       The string to wrap.
    /// \param in_maxLength The maximum length of each line.
    /// 
    /// \returns The input string with line breaks placed around word
    ///          boundaries to fit in the maximum line length.
    ///
    template <typename TStringView, typename TString = std::basic_string<typename TStringView::value_type>>
    inline TString Wrap(TStringView in_str, size_t in_maxLength);
}

#include "String.inl"
