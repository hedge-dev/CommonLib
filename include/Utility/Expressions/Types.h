#pragma once

#include <concepts>
#include <string>
#include <type_traits>

namespace hedgedev::csl::ut::expr
{
    template <typename T>
    struct GetCharType;

    ///
    /// Gets the character type of an std::basic_string.
    ///
    template <typename TChar, typename TTraits>
    struct GetCharType<std::basic_string<TChar, TTraits>> { using Type = TChar; };

    ///
    /// Gets the character type of an std::basic_string_view.
    ///
    template <typename TChar, typename TTraits>
    struct GetCharType<std::basic_string_view<TChar, TTraits>> { using Type = TChar; };

    ///
    /// Gets the character type of a C string.
    ///
    template <typename T>
    struct GetCharType<T*> { using Type = std::remove_cv_t<T>; };

    ///
    /// Gets the character type of a C string literal.
    ///
    template <typename T, size_t Count>
    struct GetCharType<T(&)[Count]> { using Type = std::remove_cv_t<T>; };

    ///
    /// Gets the character type of a string.
    ///
    template <typename T>
    using GetCharType_t = typename GetCharType<std::decay_t<T>>::Type;

    ///
    /// Determines whether the type is a C string.
    ///
    template <typename T>
    concept IsCString = std::is_same_v<GetCharType_t<T>*, char*>;

    ///
    /// Determines whether the type is a C wide string.
    ///
    template <typename T>
    concept IsCStringW = std::is_same_v<GetCharType_t<T>*, wchar_t*>;

    template <typename T>
    struct IsString : std::false_type {};
    
    ///
    /// Determines whether the type is an std::basic_string.
    ///
    template <typename TChar, typename TTraits>
    struct IsString<std::basic_string<TChar, TTraits>> : std::true_type {};

    template <typename T>
    struct IsStringView : std::false_type {};
    
    ///
    /// Determines whether the type is an std::basic_string_view.
    ///
    template <typename TChar, typename TTraits>
    struct IsStringView<std::basic_string_view<TChar, TTraits>> : std::true_type {};

    ///
    /// Determines whether the type is an std::basic_string or std::basic_string_view.
    ///
    template <typename T>
    concept IsStringOrView = IsString<std::remove_cvref_t<std::decay_t<T>>>::value || IsStringView<std::remove_cvref_t<std::decay_t<T>>>::value;
}
