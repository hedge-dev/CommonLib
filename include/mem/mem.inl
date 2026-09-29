#include <algorithm>
#include <array>
#include <string>
#include <type_traits>
#include <vector>

#if defined(_MSC_VER)
#include <stdlib.h>
#endif

#include "ut/expr/expr.h"
#include "ut/expr/string_expr.h"
#include "ut/preprocessor.h"

namespace hedgedev::csl::mem
{
    template <typename T> requires (std::is_integral_v<T>)
    inline constexpr T align(T in_address, T in_alignment)
    {
        return (in_address + (in_alignment - 1)) & ~(in_alignment - 1);
    }

    inline void* align(void* in_address, uintptr_t in_alignment)
    {
        return reinterpret_cast<void*>(align<uintptr_t>(uintptr_t(in_address), in_alignment));
    }

    template <typename T>
    inline T read(void* in_address)
    {
        return *reinterpret_cast<T*>(in_address);
    }

    template <typename T, size_t count>
    inline std::array<T, count> read(void* in_address)
    {
        std::array<T, count> result{};

        for (size_t i = 0; i < count; i++)
            result[i] = reinterpret_cast<T*>(in_address)[i];

        return result;
    }

    template <typename T>
    inline constexpr T byteswap(T in_value)
    {
        if constexpr (sizeof(T) == 1)
        {
            return in_value;
        }
        else if (std::is_constant_evaluated())
        {
            auto bytes = std::bit_cast<std::array<uint8_t, sizeof(T)>>(in_value);

            std::reverse(bytes.begin(), bytes.end());

            return std::bit_cast<T>(bytes);
        }
        else if constexpr (sizeof(T) == 2 || sizeof(T) == 4 || sizeof(T) == 8)
        {
            ut::expr::inferred_size_t<sizeof(T)> buffer{};

            memcpy(&buffer, &in_value, sizeof(T));

            if constexpr (sizeof(T) == 2)
            {
#if defined(_MSC_VER)
                buffer = _byteswap_ushort(buffer);
#else
                buffer = __builtin_bswap16(buffer);
#endif
            }
            else if constexpr (sizeof(T) == 4)
            {
#if defined(_MSC_VER)
                buffer = _byteswap_ulong(buffer);
#else
                buffer = __builtin_bswap32(buffer);
#endif
            }
            else if constexpr (sizeof(T) == 8)
            {
#if defined(_MSC_VER)
                buffer = _byteswap_uint64(buffer);
#else
                buffer = __builtin_bswap64(buffer);
#endif
            }

            memcpy(&in_value, &buffer, sizeof(T));
        }
        else
        {
            auto bytes = reinterpret_cast<uint8_t*>(&in_value);

            std::reverse(bytes, bytes + sizeof(T));
        }

        return in_value;
    }

    template <typename T>
    inline constexpr void byteswap_inplace(T& io_value)
    {
        io_value = byteswap(io_value);
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
            reinterpret_cast<T*>(in_address)[i] = in_data;
        
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

    template <ut::expr::any_string T>
    inline bool write_string(void* in_address, const T& in_str)
    {
        if (!in_address)
            return false;

        return write_string_fixed_length(in_address, in_str, ut::expr::inferred_string_view_t<T>(in_str).size());
    }

    template <ut::expr::any_string T>
    inline bool write_string_fixed_length(void* in_address, const T& in_str, size_t in_length)
    {
        if (!in_address)
            return false;

        using str_char_t = ut::expr::get_char_type_t<T>;

        auto str_sv = ut::expr::inferred_string_view_t<T>(in_str);

        auto src_length = in_length;
        auto dst_length = src_length;

        if (!dst_length)
        {
            // Started at null terminator, abort.
            if (!*reinterpret_cast<str_char_t*>(in_address))
                return false;

            dst_length = ut::expr::inferred_string_view_t<T>(reinterpret_cast<str_char_t*>(in_address)).size() * sizeof(str_char_t);

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
        memset(reinterpret_cast<char*>(uintptr_t(in_address) + src_length), 0, sizeof(str_char_t));
        ASSERT_RETURN_FALSE(protect(in_address, dst_length, old_protect_flags));

        return true;
    }
}
