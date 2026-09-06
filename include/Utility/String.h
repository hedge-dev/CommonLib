#pragma once

#include <optional>
#include <string>
#include <vector>

namespace hedgedev::csl::ut::string
{
    ///
    /// Compares two strings.
    ///
    /// \param in_rLeft           The first string.
    /// \param in_rRight          The second string.
    /// \param in_isCaseSensitive Determines whether the comparison should be case sensitive.
    /// 
    /// \returns `true` if the strings are identical. Otherwise, `false`.
    ///
    template <expr::AnyString TLeft, expr::AnyString TRight>
    inline bool Compare(const TLeft& in_rLeft, const TRight& in_rRight, bool in_isCaseSensitive = true);

    ///
    /// Checks if a string contains a substring.
    ///
    /// \param in_rStr            The string to check.
    /// \param in_rSubStr         The substring to search for.
    /// \param in_isCaseSensitive Determines whether the search should be case sensitive.
    /// 
    /// \returns `true` if the input string contains the specified substring. Otherwise, `false`.
    ///
    template <expr::AnyString TString, expr::AnyString TSubString>
    inline bool Contains(const TString& in_rStr, const TSubString& in_rSubStr, bool in_isCaseSensitive = true);

    ///
    /// Converts a string to a different encoding format.
    /// 
    /// \tparam TDst The string type to convert to.
    /// \tparam TSrc The string type to convert from.
    ///
    /// \param in_rStr  The string to convert.
    /// \param out_rDst The output string to set.
    /// 
    /// \returns If successful, the input string in the destination format.
    ///          Otherwise, an empty string in the destination format.
    ///
    template <expr::AnyString TDst, expr::AnyString TSrc>
    inline std::conditional_t<std::is_same_v<TDst, TSrc> && !std::is_pointer_v<TSrc>, const TDst&, TDst> Convert(const TSrc& in_rStr);

    ///
    /// Escapes all instances of a character in a string with an escape sequence.
    /// 
    /// \tparam TString     The input string type.
    /// \tparam TChar       The search and escape character type inferred from the input string type.
    ///
    /// \param in_rStr       The string to escape.
    /// \param in_searchChar The character to search for and escape.
    /// \param in_escapeChar The character to escape with.
    /// 
    /// \returns The input string where all characters matching the search character have been escaped using the escape character.
    ///
    template <expr::AnyString TString, typename TChar = expr::GetCharType_t<TString>>
    inline expr::InferredString<TString> Escape(const TString& in_rStr, TChar in_searchChar, TChar in_escapeChar = TChar('\\'));

    ///
    /// Formats a string.
    /// 
    /// \tparam TString The input string type.
    ///
    /// \param in_str The string to format.
    /// 
    /// \returns The input string with the formatters filled in.
    ///
    template <expr::AnyString TString>
    inline expr::InferredString<TString> Format(const TString in_str, ...);

    ///
    /// Gets the width of a string.
    /// 
    /// \tparam TString The input string type.
    ///
    /// \param in_rStr The string to search.
    /// 
    /// \returns The width of the string in characters determined by the longest line.
    ///
    template <expr::AnyString TString>
    inline size_t GetWidth(const TString& in_rStr);

    ///
    /// Formats a string to have a HTML hyperlink.
    /// 
    /// \tparam TString The input string type.
    ///
    /// \param in_rStr The string to display.
    /// \param in_rUrl The URL to use.
    /// 
    /// \returns The input string formatted with a HTML hyperlink with a URL target.
    ///
    template <expr::AnyString TString, expr::AnyString TUrl, typename TOut = expr::PrecedentInferredString<TString, TUrl>>
    inline TOut Hyperlink(const TString& in_rStr, const TUrl& in_rUrl);

    ///
    /// Joins a collection of strings together using a delimiter.
    /// 
    /// \tparam TDelimiter  The delimiter string type.
    /// \tparam TCollection The collection string type.
    ///
    /// \param in_pDelimiter The delimiter to join the strings with.
    /// \param in_rStrings   The strings to join.
    /// 
    /// \returns The input strings joined together, separated by the delimiter.
    ///
    template <expr::AnyString TDelimiter, expr::AnyString TCollection>
    inline expr::InferredString<TCollection> Join(const TDelimiter& in_rDelimiter, const std::vector<TCollection>& in_rStrings);

    ///
    /// Joins a collection of strings together using a delimiter.
    /// 
    /// \tparam TDelimiter The delimiter string type.
    /// \tparam TArgs      The string types.
    ///
    /// \param in_pDelimiter The delimiter to join the strings with.
    /// \param in_rArgs      The strings to join.
    /// 
    /// \returns The input strings joined together, separated by the delimiter.
    ///
    template <expr::AnyString TDelimiter, expr::AnyString... TArgs, typename TOut = expr::PrecedentInferredString<TDelimiter, TArgs...>>
    inline TOut Join(const TDelimiter& in_rDelimiter, const TArgs&... in_rArgs);

    ///
    /// Pads a string with another.
    /// 
    /// \tparam TString    The input string type.
    /// \tparam TPadString The pad string type.
    ///
    /// \param in_rStr    The string to pad.
    /// \param in_rPadStr The string to pad with.
    /// 
    /// \returns The input string padded on either side with the pad string.
    ///
    template <expr::AnyString TString, expr::AnyString TPadString, typename TOut = expr::PrecedentInferredString<TString, TPadString>>
    inline TOut Pad(const TString& in_rStr, const TPadString& in_rPadStr);

    ///
    /// Parses a value from a string.
    /// 
    /// \tparam TOut    The value type to parse.
    /// \tparam TString The input string type.
    ///
    /// \param in_rStr The string to parse.
    /// 
    /// \returns The value that was parsed.
    ///
    template <typename TOut, expr::AnyString TString>
    inline TOut Parse(const TString& in_rStr);

    ///
    /// Converts multiple strings to share the same encoding format.
    /// 
    /// \tparam TArgs The string types.
    /// \tparam TOut  The string type to convert to inferred from the string with the largest character size.
    ///
    /// \param in_rArgs The strings to convert.
    /// 
    /// \returns An array of strings in the same destination format.
    ///          If a string failed to convert, an empty string will be put in its place.
    ///
    template <expr::AnyString... TArgs, typename TOut = expr::PrecedentInferredString<TArgs...>>
    inline std::array<TOut, sizeof...(TArgs)> PrecedentConvert(const TArgs&... in_rArgs);

    ///
    /// Omits XML tags from a string.
    /// 
    /// \tparam TString The input string type.
    ///
    /// \param in_str The string to omit XML tags from.
    /// 
    /// \returns The input string with XML tags removed.
    ///
    template <expr::AnyString TString>
    inline expr::InferredString<TString> RemoveXmlTags(const TString& in_rStr);

    ///
    /// Splits a string into a collection using a delimiter.
    /// 
    /// \tparam TString    The input string type.
    /// \tparam TDelimiter The delimiter string type.
    ///
    /// \param in_rStr       The string to split.
    /// \param in_rDelimiter The delimiter to split the string with.
    /// 
    /// \returns A collection of strings split by the delimiter.
    ///
    template <expr::AnyString TString, expr::AnyString TDelimiter, typename TOut = expr::PrecedentInferredString<TString, TDelimiter>>
    inline std::vector<TOut> Split(const TString& in_rStr, const TDelimiter& in_rDelimiter);

    ///
    /// Transforms a string to lowercase.
    /// 
    /// \tparam TString The input string type.
    ///
    /// \param in_rStr The string to transform to lowercase.
    /// 
    /// \returns The input string as lowercase.
    ///
    template <expr::AnyString TString>
    inline expr::InferredString<TString> ToLower(const TString& in_rStr);

    ///
    /// Transforms a string to uppercase.
    /// 
    /// \tparam TString The input string type.
    ///
    /// \param in_rStr The string to transform to uppercase.
    /// 
    /// \returns The input string as uppercase.
    ///
    template <expr::AnyString TString>
    inline expr::InferredString<TString> ToUpper(const TString& in_rStr);

    ///
    /// Removes all leading occurrences of a set of characters from the input string.
    /// 
    /// \tparam TString The input string type.
    /// \tparam TChar   The trim character type inferred from the input string type.
    ///
    /// \param in_rStr      The string to trim.
    /// \param in_trimChars The characters to trim off.
    /// 
    /// \returns The input string with all occurrences of the specified characters removed from the start.
    ///
    template <expr::AnyString TString, typename TChar = expr::GetCharType_t<TString>>
    inline expr::InferredString<TString> TrimStart(const TString& in_rStr, std::optional<std::vector<TChar>> in_trimChars = {});

    ///
    /// Removes all trailing occurrences of a set of characters from the input string.
    /// 
    /// \tparam TString The input string type.
    /// \tparam TChar   The trim character type inferred from the input string type.
    ///
    /// \param in_rStr      The string to trim.
    /// \param in_trimChars The characters to trim off.
    /// 
    /// \returns The input string with all occurrences of the specified characters removed from the end.
    ///
    template <expr::AnyString TString, typename TChar = expr::GetCharType_t<TString>>
    inline expr::InferredString<TString> TrimEnd(const TString& in_rStr, std::optional<std::vector<TChar>> in_trimChars = {});

    ///
    /// Removes all leading and trailing occurrences of a set of characters from the input string.
    /// 
    /// \tparam TString The input string type.
    /// \tparam TChar   The trim character type inferred from the input string type.
    ///
    /// \param in_rStr      The string to trim.
    /// \param in_trimChars The characters to trim off.
    /// 
    /// \returns The input string with all occurrences of the specified characters removed from the start and end.
    ///
    template <expr::AnyString TString, typename TChar = expr::GetCharType_t<TString>>
    inline expr::InferredString<TString> Trim(const TString& in_rStr, std::optional<std::vector<TChar>> in_trimChars = {});

    ///
    /// Truncates a string.
    /// 
    /// \tparam TString The input string type.
    ///
    /// \param in_rStr        The string to truncate.
    /// \param in_maxLength   The maximum length to truncate to.
    /// \param in_truncateEnd Determines whether to truncate the end of the string, rather than the beginning.
    /// \param in_ellipsis    Determines whether to add an ellipsis to the truncated side of the string.
    /// 
    /// \returns The input string truncated to the specified length.
    ///
    template <expr::AnyString TString>
    inline expr::InferredString<TString> Truncate(const TString& in_rStr, size_t in_maxLength, bool in_truncateEnd = true, bool in_ellipsis = true);

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
    template <expr::BasicString TString, typename TValue>
    inline TString ToHex(TValue in_value, size_t in_maxLength = sizeof(size_t) * 2, bool in_prefix = true);

    ///
    /// Tries to convert a string to a different encoding format.
    /// 
    /// \tparam TDst The string type to convert to.
    /// \tparam TSrc The string type to convert from.
    ///
    /// \param in_rSrc  The string to convert.
    /// \param out_rDst The output string to set.
    /// 
    /// \returns `true` if the conversion was successful. Otherwise, `false`.
    ///
    template <expr::AnyString TDst, expr::AnyString TSrc>
    inline bool TryConvert(const TSrc& in_rSrc, TDst& out_rDst);

    ///
    /// Tries to parse a value from a string.
    /// 
    /// \tparam TOut    The value type to parse.
    /// \tparam TString The input string type.
    ///
    /// \param in_rStr    The string to parse.
    /// \param out_rValue The output value to set.
    /// 
    /// \returns `true` if the parse was successful. Otherwise, `false`.
    ///
    template <typename TOut, expr::AnyString TString>
    inline bool TryParse(const TString& in_rStr, TOut& out_rValue);

    ///
    /// Wraps a string to a fixed character length on each line.
    /// 
    /// \tparam TString The input string type.
    ///
    /// \param in_rStr     The string to wrap.
    /// \param in_maxWidth The maximum width of each line.
    /// 
    /// \returns The input string with line breaks placed around word
    ///          boundaries to fit within the maximum line width.
    ///
    template <expr::AnyString TString>
    inline expr::InferredString<TString> Wrap(const TString& in_rStr, size_t in_maxWidth);
}

#include "String.inl"

__CMNLIB_INTERNAL_MAKE_NAMESPACE_ALIAS(hedgedev::csl::ut, string, str);
