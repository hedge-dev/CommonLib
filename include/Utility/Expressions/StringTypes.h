#pragma once

#include <concepts>
#include <string>
#include <tuple>
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
    /// Checks if the type is an `std::basic_string`.
    ///
    template <typename T>
    inline constexpr bool IsBasicString_v = IsBasicString<std::remove_cvref_t<std::decay_t<T>>>::value;

    ///
    /// Checks if the type is an `std::basic_string_view`.
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
    /// An `std::basic_string` type.
    ///
    template <typename T>
    concept BasicString = IsBasicString_v<T>;

    ///
    /// An `std::basic_string_view` type.
    ///
    template <typename T>
    concept BasicStringView = IsBasicStringView_v<T>;

    ///
    /// A multibyte or wide C string type, or an `std::basic_string` or `std::basic_string_view` type.
    ///
    template <typename T>
    concept AnyString = CString<T> || BasicString<T> || BasicStringView<T>;

    ///
    /// Checks if the underlying type of two given string types are the same.
    ///
    template <AnyString T1, AnyString T2>
    inline constexpr bool IsSameUnderlyingCharType = std::is_same_v<GetCharType_t<T1>, GetCharType_t<T2>>;

    ///
    /// An `std::basic_string` inferred from any string type.
    ///
    template <AnyString T>
    using InferredString = std::basic_string<GetCharType_t<T>>;

    ///
    /// An `std::basic_string_view` inferred from any string type.
    ///
    template <AnyString T>
    using InferredStringView = std::basic_string_view<GetCharType_t<T>>;

    ///
    /// An `std::basic_string` or `std::basic_string_view` inferred from the string type.
    /// 
    /// \returns `std::basic_string_view` if the type is a C string. Otherwise, `std::basic_string`.
    ///
    template <AnyString T>
    using InferredStringOrView = std::conditional_t<CString<T>, InferredStringView<T>, InferredString<T>>;

    ///
    /// Creates a string inferred from a string literal at compile time.
    /// 
    /// \tparam TDst The string type to create.
    /// \tparam TSrc The string type to convert from.
    ///
    /// \param in_rStr The string to create.
    /// 
    /// \returns The input string in the destination format.
    ///
    template <AnyString TDst, AnyString TSrc>
    inline constexpr TDst CreateInferredString(const TSrc& in_rStr);

    ///
    /// Determines which string type has the largest character size.
    /// 
    /// \returns The index of the string type with the largest character size.
    ///
    template <AnyString... TArgs>
    inline constexpr size_t GetStringTypePrecedence();

    ///
    /// Evaluates the string type with the largest character size.
    ///
    template <AnyString... TArgs>
    using PrecedentString = std::tuple_element_t<GetStringTypePrecedence<TArgs...>(), std::tuple<TArgs...>>;

    ///
    /// An `std::basic_string` inferred from the string type with the largest character size.
    ///
    template <AnyString... TArgs>
    using PrecedentInferredString = InferredString<PrecedentString<TArgs...>>;

    ///
    /// An `std::basic_string_view` inferred from the string type with the largest character size.
    ///
    template <AnyString... TArgs>
    using PrecedentInferredStringView = InferredStringView<PrecedentString<TArgs...>>;

    ///
    /// An `std::basic_string` or `std::basic_string_view` inferred from the string type with the largest character size.
    /// 
    /// \returns `std::basic_string_view` if all types are C strings. Otherwise, `std::basic_string`.
    ///
    template <AnyString... TArgs>
    using PrecedentInferredStringOrView = std::conditional_t<(IsAllSame<TArgs...> && CString<GetPackType<0, TArgs...>>),
        PrecedentInferredStringView<TArgs...>,
        PrecedentInferredString<TArgs...>>;
}

#include "StringTypes.inl"
