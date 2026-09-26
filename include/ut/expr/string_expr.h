#pragma once

#include <concepts>
#include <fstream>
#include <ranges>
#include <sstream>
#include <string>
#include <tuple>
#include <type_traits>

#include "container_expr.h"

namespace hedgedev::csl::ut::expr
{
    ///
    /// Gets the character type of a non-range type.
    ///
    template <typename T, typename = void>
    struct get_char_type { using type = std::remove_cv_t<std::remove_pointer_t<std::decay_t<T>>>; };

    ///
    /// Gets the character type of a range type.
    ///
    template <typename T>
    struct get_char_type<T, std::enable_if_t<std::ranges::range<std::remove_cvref_t<T>> && !std::is_pointer_v<std::decay_t<T>>>> { using type = std::ranges::range_value_t<std::remove_cvref_t<T>>; };

    ///
    /// Gets the character type of a string container.
    ///
    template <typename T>
    using get_char_type_t = typename get_char_type<T>::type;

    ///
    /// A multibyte C string type.
    ///
    template <typename T>
    concept raw_string_c = std::is_same_v<get_char_type_t<T>, char>;

    ///
    /// A wide C string type.
    ///
    template <typename T>
    concept raw_string_w = std::is_same_v<get_char_type_t<T>, wchar_t>;

    ///
    /// An 8-bit C string type.
    ///
    template <typename T>
    concept raw_string_u8 = std::is_same_v<get_char_type_t<T>, char8_t>;

    ///
    /// A 16-bit C string type.
    ///
    template <typename T>
    concept raw_string_u16 = std::is_same_v<get_char_type_t<T>, char16_t>;

    ///
    /// A 32-bit C string type.
    ///
    template <typename T>
    concept raw_string_u32 = std::is_same_v<get_char_type_t<T>, char32_t>;

    template <typename T>
    struct is_basic_string : std::false_type {};

    template <typename T_char, typename T_traits>
    struct is_basic_string<std::basic_string<T_char, T_traits>> : std::true_type {};

    ///
    /// An `std::basic_string` type.
    ///
    template <typename T>
    concept basic_string = is_basic_string<std::remove_cvref_t<std::decay_t<T>>>::value;

    template <typename T>
    struct is_basic_string_view : std::false_type {};
    
    template <typename T_char, typename T_traits>
    struct is_basic_string_view<std::basic_string_view<T_char, T_traits>> : std::true_type {};

    ///
    /// An `std::basic_string_view` type.
    ///
    template <typename T>
    concept basic_string_view = is_basic_string_view<std::remove_cvref_t<std::decay_t<T>>>::value;

    ///
    /// Any raw string type.
    ///
    template <typename T>
    concept any_raw_string = raw_string_c<T> || raw_string_w<T> || raw_string_u8<T> || raw_string_u16<T> || raw_string_u32<T>;

    ///
    /// Any string type.
    ///
    template <typename T>
    concept any_string = any_raw_string<T> || basic_string<T> || basic_string_view<T>;

    ///
    /// A wide C string or `std::wstring` type.
    ///
    template <typename T>
    concept any_string_w = raw_string_w<T> || std::is_same_v<T, std::wstring>;

    ///
    /// An 8-bit C string or `std::u8string` type.
    ///
    template <typename T>
    concept any_string_u8 = raw_string_u8<T> || std::is_same_v<T, std::u8string>;

    ///
    /// A 16-bit C string or `std::u16string` type.
    ///
    template <typename T>
    concept any_string_u16 = raw_string_u16<T> || std::is_same_v<T, std::u16string>;

    ///
    /// A 32-bit C string or `std::u32string` type.
    ///
    template <typename T>
    concept any_string_u32 = raw_string_u32<T> || std::is_same_v<T, std::u32string>;

    ///
    /// Checks if the underlying type of two given string types are the same.
    ///
    template <any_string T_left, any_string T_right>
    inline constexpr bool is_same_underlying_char_type_v = std::is_same_v<get_char_type_t<T_left>, get_char_type_t<T_right>>;

    ///
    /// An `std::basic_string` inferred from any string type.
    ///
    template <any_string T>
    using inferred_string_t = std::basic_string<get_char_type_t<T>>;

    ///
    /// An `std::basic_string_view` inferred from any string type.
    ///
    template <any_string T>
    using inferred_string_view_t = std::basic_string_view<get_char_type_t<T>>;

    ///
    /// An `std::basic_stringstream` inferred from any string type.
    ///
    template <any_string T>
    using inferred_stringstream_t = std::basic_stringstream<get_char_type_t<T>>;

    ///
    /// An `std::basic_fstream` inferred from any string type.
    ///
    template <any_string T>
    using inferred_fstream_t = std::basic_fstream<std::conditional_t<std::is_same_v<get_char_type_t<T>, char8_t>, char, get_char_type_t<T>>>;

    ///
    /// Creates a string inferred from a string literal at compile time.
    ///
    /// \tparam T_result The string type to create.
    /// \tparam T_str The string type to convert from.
    ///
    /// \param in_str The string to create.
    ///
    /// \returns The input string in the destination format.
    ///
    template <any_string T_result, any_string T_str>
    inline constexpr T_result create_inferred_string(const T_str& in_str);

    ///
    /// Determines which string type has the largest character size.
    ///
    /// \returns The index of the string type with the largest character size.
    ///
    template <any_string... T_args>
    inline constexpr size_t get_string_type_precedence();

    ///
    /// Evaluates the string type with the largest character size.
    ///
    template <any_string... T_args>
    using precedent_string_t = std::tuple_element_t<get_string_type_precedence<T_args...>(), std::tuple<T_args...>>;

    ///
    /// An `std::basic_string` inferred from the string type with the largest
    /// character size.
    ///
    template <any_string... T_args>
    using pi_string_t = inferred_string_t<precedent_string_t<T_args...>>;

    ///
    /// An `std::basic_string_view` inferred from the string type with the largest
    /// character size.
    ///
    template <any_string... T_args>
    using pi_string_view_t = inferred_string_view_t<precedent_string_t<T_args...>>;

    ///
    /// An `std::basic_stringstream` inferred from the string type with the largest
    /// character size.
    ///
    template <any_string... T_args>
    using pi_stringstream_t = inferred_stringstream_t<precedent_string_t<T_args...>>;
}

#include "string_expr.inl"
