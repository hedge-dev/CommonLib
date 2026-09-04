#pragma once

#include <concepts>
#include <string>
#include <type_traits>

namespace hedgedev::csl::ut::expr
{
    template <typename T>
    struct GetCharType;
    
    template <typename TChar, typename TTraits>
    struct GetCharType<std::basic_string<TChar, TTraits>> { using Type = TChar; };
    
    template <typename TChar, typename TTraits>
    struct GetCharType<std::basic_string_view<TChar, TTraits>> { using Type = TChar; };
    
    template <typename T>
    struct GetCharType<T*> { using Type = std::remove_cv_t<T>; };
    
    template <typename T, size_t Count>
    struct GetCharType<T(&)[Count]> { using Type = std::remove_cv_t<T>; };

    template <typename T>
    struct IsBasicString : std::false_type {};

    template <typename TChar, typename TTraits>
    struct IsBasicString<std::basic_string<TChar, TTraits>> : std::true_type {};

    template <typename T>
    struct IsBasicStringView : std::false_type {};
    
    template <typename TChar, typename TTraits>
    struct IsBasicStringView<std::basic_string_view<TChar, TTraits>> : std::true_type {};

    ///
    /// Gets the character type of a string.
    ///
    template <typename T>
    using GetCharType_t = typename GetCharType<std::decay_t<T>>::Type;
    
    ///
    /// Checks if the type is an std::basic_string.
    ///
    template <typename T>
    inline constexpr bool IsBasicString_v = IsBasicString<std::remove_cvref_t<std::decay_t<T>>>::value;

    ///
    /// Checks if the type is an std::basic_string_view.
    ///
    template <typename T>
    inline constexpr bool IsBasicStringView_v = IsBasicStringView<std::remove_cvref_t<std::decay_t<T>>>::value;

    ///
    /// A multibyte C string type.
    ///
    template <typename T>
    concept CStringA = std::is_same_v<GetCharType_t<T>, char>;

    ///
    /// A wide C string type.
    ///
    template <typename T>
    concept CStringW = std::is_same_v<GetCharType_t<T>, wchar_t>;

    ///
    /// A multibyte or wide C string type.
    ///
    template <typename T>
    concept CString = CStringA<T> || CStringW<T>;

    ///
    /// An std::basic_string type.
    ///
    template <typename T>
    concept BasicString = IsBasicString_v<T>;

    ///
    /// An std::basic_string_view type.
    ///
    template <typename T>
    concept BasicStringView = IsBasicStringView_v<T>;

    ///
    /// A multibyte or wide C string type, or an std::basic_string or std::basic_string_view type.
    ///
    template <typename T>
    concept AnyString = CString<T> || BasicString<T> || BasicStringView<T>;
}
