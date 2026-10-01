#include <bit>

#include "ut/encoding_type.h"
#include "string_expr.h"

namespace hedgedev::csl::ut::expr
{
    template <any_string T>
    inline constexpr encoding::encoding_type get_char_encoding_type()
    {
        using char_type_t = get_char_type_t<T>;

        if constexpr (sizeof(char_type_t) == 1)
        {
            return encoding::utf8;
        }
        else if constexpr (sizeof(char_type_t) == 2)
        {
            if constexpr (std::endian::native == std::endian::little)
            {
                return encoding::utf16_le;
            }
            else
            {
                return encoding::utf16_be;
            }
        }
        else if constexpr (sizeof(char_type_t) == 4)
        {
            if constexpr (std::endian::native == std::endian::little)
            {
                return encoding::utf32_le;
            }
            else
            {
                return encoding::utf32_be;
            }
        }

        return encoding::unknown;
    }

    inline constexpr encoding::encoding_type get_native_encoding_type(encoding::encoding_type in_encoding)
    {
        if constexpr (std::endian::native == std::endian::little)
        {
            switch (in_encoding)
            {
                case encoding::utf16_be:
                    return encoding::utf16_le;

                case encoding::utf32_be:
                    return encoding::utf32_le;
            }
        }
        else
        {
            switch (in_encoding)
            {
                case encoding::utf16_le:
                    return encoding::utf16_be;

                case encoding::utf32_le:
                    return encoding::utf32_be;
            }
        }
        
        return in_encoding;
    }

    inline constexpr size_t get_encoding_char_size(encoding::encoding_type in_encoding)
    {
        switch (in_encoding)
        {
            case encoding::utf8:
            case encoding::latin1:
                return sizeof(char8_t);

            case encoding::utf16_le:
            case encoding::utf16_be:
                return sizeof(char16_t);

            case encoding::utf32_le:
            case encoding::utf32_be:
                return sizeof(char32_t);
        }

        return 0;
    }

    template <any_string T>
    inline constexpr bool is_char_encoding_type(encoding::encoding_type in_encoding)
    {
        return get_char_encoding_type<T>() == in_encoding;
    }
}
