#include <bit>

#include "ut/encoding.h"
#include "string_expr.h"

namespace hedgedev::csl::ut::expr
{
    inline constexpr size_t get_encoding_char_size(encoding::encoding_type in_encoding)
    {
        switch (in_encoding)
        {
            case encoding::encoding_type::utf8:
            case encoding::encoding_type::latin1:
                return sizeof(char8_t);

            case encoding::encoding_type::utf16_le:
            case encoding::encoding_type::utf16_be:
                return sizeof(char16_t);

            case encoding::encoding_type::utf32_le:
            case encoding::encoding_type::utf32_be:
                return sizeof(char32_t);
        }

        return 0;
    }

    template <typename T>
    inline constexpr encoding::encoding_type get_char_encoding_type()
    {
        using char_type_t = get_char_type_t<T>;

        if constexpr (sizeof(char_type_t) == 1)
        {
            return encoding::encoding_type::utf8;
        }
        else if constexpr (sizeof(char_type_t) == 2)
        {
            if constexpr (std::endian::native == std::endian::little)
            {
                return encoding::encoding_type::utf16_le;
            }
            else
            {
                return encoding::encoding_type::utf16_be;
            }
        }
        else if constexpr (sizeof(char_type_t) == 4)
        {
            if constexpr (std::endian::native == std::endian::little)
            {
                return encoding::encoding_type::utf32_le;
            }
            else
            {
                return encoding::encoding_type::utf32_be;
            }
        }

        return encoding::encoding_type::unknown;
    }
}
