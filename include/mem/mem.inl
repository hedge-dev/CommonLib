#include <array>
#include <string>
#include <vector>

#include "ut/preprocessor.h"

namespace hedgedev::csl::mem
{
    template <typename T>
    inline T read(void* in_address)
    {
        return *(T*)in_address;
    }

    template <typename T, size_t K_count>
    inline std::array<T, K_count> read(void* in_address)
    {
        std::array<T, K_count> result{};

        for (size_t i = 0; i < K_count; i++)
            result[i] = ((T*)in_address)[i];

        return result;
    }

    template <typename T>
    inline bool write(void* in_address, const T& in_data, size_t in_count)
    {
        if (!in_address)
            return false;

        const auto length = sizeof(in_data) * in_count;
        
        uint32_t old_protect_flags{};

        ASSERT_RETURN_FALSE(protect(in_address, length, get_protect_flags(page_protection::rw), &old_protect_flags));

        for (size_t i = 0; i < in_count; i++)
            ((T*)in_address)[i] = in_data;
        
        ASSERT_RETURN_FALSE(protect(in_address, length, old_protect_flags));

        return true;
    }

    template <typename T>
    inline bool write(void* in_address, const std::vector<T>& in_data)
    {
        if (!in_address)
            return false;

        const auto length = sizeof(T) * in_data.size();

        uint32_t old_protect_flags{};
        
        ASSERT_RETURN_FALSE(protect(in_address, length, get_protect_flags(page_protection::rw), &old_protect_flags));
        ASSERT_RETURN_FALSE(memcpy_s(in_address, length, in_data.data(), length) == 0);
        ASSERT_RETURN_FALSE(protect(in_address, length, old_protect_flags));

        return true;
    }

    template <ut::expr::any_string_t T>
    inline bool write_string(void* in_address, const T& in_str)
    {
        if (!in_address)
            return false;

        using str_char_t = ut::expr::get_char_type_t<T>;

        return write_string_fixed_length(in_address, in_str, std::basic_string_view<str_char_t>(in_str).size());
    }

    template <ut::expr::any_string_t T>
    inline bool write_string_fixed_length(void* in_address, const T& in_str, size_t in_length)
    {
        if (!in_address)
            return false;

        using str_char_t = ut::expr::get_char_type_t<T>;

        auto str_sv = std::basic_string_view<str_char_t>(in_str);

        auto src_length = in_length;
        auto dst_length = src_length;

        if (dst_length <= 0)
        {
            // Started at null terminator, abort.
            if (!*(str_char_t*)in_address)
                return false;

            dst_length = std::basic_string_view<str_char_t>((const str_char_t*)in_address).size() * sizeof(str_char_t);

            if (!dst_length)
                return false;

            src_length = str_sv.size() * sizeof(str_char_t);

            if (!src_length)
                return false;

            // Use the smallest length to fit
            // within existing string boundaries.
            src_length = std::min(src_length, dst_length);
        }

        uint32_t old_protect_flags{};

        ASSERT_RETURN_FALSE(protect(in_address, dst_length, get_protect_flags(page_protection::rw), &old_protect_flags));
        ASSERT_RETURN_FALSE(memcpy_s(in_address, dst_length, str_sv.data(), src_length) == 0);
        memset((char*)(size_t(in_address) + src_length), 0, sizeof(str_char_t));
        ASSERT_RETURN_FALSE(protect(in_address, dst_length, old_protect_flags));

        return true;
    }
}
