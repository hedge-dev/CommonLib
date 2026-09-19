#pragma once

#include <concepts>
#include <string>
#include <tuple>
#include <type_traits>

namespace hedgedev::csl::ut::expr
{
    template <typename T>
    struct get_char_type;
    
    template <typename T_char, typename T_traits>
    struct get_char_type<std::basic_string<T_char, T_traits>> { using type = T_char; };
    
    template <typename T_char, typename T_traits>
    struct get_char_type<std::basic_string_view<T_char, T_traits>> { using type = T_char; };
    
    template <typename T>
    struct get_char_type<T*> { using type = std::remove_cv_t<T>; };
    
    template <typename T, size_t count>
    struct get_char_type<T(&)[count]> { using type = std::remove_cv_t<T>; };

    template <typename T>
    struct is_basic_string : std::false_type {};

    template <typename T_char, typename T_traits>
    struct is_basic_string<std::basic_string<T_char, T_traits>> : std::true_type {};

    template <typename T>
    struct is_basic_string_view : std::false_type {};
    
    template <typename T_char, typename T_traits>
    struct is_basic_string_view<std::basic_string_view<T_char, T_traits>> : std::true_type {};

    ///
    /// Gets the character type of a string.
    ///
    template <typename T>
    using get_char_type_t = typename get_char_type<std::decay_t<T>>::type;
    
    ///
    /// Checks if the type is an `std::basic_string`.
    ///
    template <typename T>
    inline constexpr bool is_basic_string_v = is_basic_string<std::remove_cvref_t<std::decay_t<T>>>::value;

    ///
    /// Checks if the type is an `std::basic_string_view`.
    ///
    template <typename T>
    inline constexpr bool is_basic_string_view_v = is_basic_string_view<std::remove_cvref_t<std::decay_t<T>>>::value;

    ///
    /// A multibyte C string type.
    ///
    template <typename T>
    concept raw_string_c_t = std::is_same_v<get_char_type_t<T>, char>;

    ///
    /// A wide C string type.
    ///
    template <typename T>
    concept raw_string_w_t = std::is_same_v<get_char_type_t<T>, wchar_t>;

    ///
    /// A multibyte or wide C string type.
    ///
    template <typename T>
    concept raw_string_t = raw_string_c_t<T> || raw_string_w_t<T>;

    ///
    /// An `std::basic_string` type.
    ///
    template <typename T>
    concept basic_string_t = is_basic_string_v<T>;

    ///
    /// An `std::basic_string_view` type.
    ///
    template <typename T>
    concept basic_string_view_t = is_basic_string_v<T>;

    ///
    /// A multibyte or wide C string type, or an `std::basic_string` or
    /// `std::basic_string_view` type.
    ///
    template <typename T>
    concept any_string_t = raw_string_t<T> || basic_string_t<T> || basic_string_view_t<T>;

    ///
    /// A wide C string or `std::wstring` type.
    ///
    template <typename T>
    concept any_string_w_t = raw_string_w_t<T> || std::is_same_v<T, std::wstring>;

    ///
    /// Checks if the underlying type of two given string types are the same.
    ///
    template <any_string_t T_left, any_string_t T_right>
    inline constexpr bool is_same_underlying_char_type_v = std::is_same_v<get_char_type_t<T_left>, get_char_type_t<T_right>>;

    ///
    /// An `std::basic_string` inferred from any string type.
    ///
    template <any_string_t T>
    using inferred_string_t = std::basic_string<get_char_type_t<T>>;

    ///
    /// An `std::basic_string_view` inferred from any string type.
    ///
    template <any_string_t T>
    using inferred_string_view_t = std::basic_string_view<get_char_type_t<T>>;

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
    template <any_string_t T_result, any_string_t T_str>
    inline constexpr T_result create_inferred_string(const T_str& in_str);

    ///
    /// Determines which string type has the largest character size.
    ///
    /// \returns The index of the string type with the largest character size.
    ///
    template <any_string_t... T_args>
    inline constexpr size_t get_string_type_precedence();

    ///
    /// Evaluates the string type with the largest character size.
    ///
    template <any_string_t... T_args>
    using precedent_string_t = std::tuple_element_t<get_string_type_precedence<T_args...>(), std::tuple<T_args...>>;

    ///
    /// An `std::basic_string` inferred from the string type with the largest
    /// character size.
    ///
    template <any_string_t... T_args>
    using pi_string_t = inferred_string_t<precedent_string_t<T_args...>>;

    ///
    /// An `std::basic_string_view` inferred from the string type with the largest
    /// character size.
    ///
    template <any_string_t... T_args>
    using pi_string_view_t = inferred_string_view_t<precedent_string_t<T_args...>>;
}

#include "string_types.inl"
