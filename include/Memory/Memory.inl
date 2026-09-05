#include <string>

namespace hedgedev::csl::mem
{
    template <typename T>
    inline T Read(void* in_pAddress)
    {
        return *(T*)in_pAddress;
    }

    template <typename T, size_t Count>
    inline std::array<T, Count> Read(void* in_pAddress)
    {
        std::array<T, Count> result{};

        for (size_t i = 0; i < Count; i++)
            result[i] = ((T*)in_pAddress)[i];

        return result;
    }

    template <typename T>
    inline bool Write(void* in_pAddress, const T& in_rData, size_t in_count)
    {
        if (!in_pAddress)
            return false;

        uint32_t oldProtectionFlags{};

        ASSERT_RETURN_FALSE(Protect(in_pAddress, sizeof(in_rData), GetProtectionFlags(PageProtection::RW), &oldProtectionFlags));

        for (size_t i = 0; i < in_count; i++)
            ((T*)in_pAddress)[i] = in_rData;
        
        ASSERT_RETURN_FALSE(Protect(in_pAddress, sizeof(in_rData), oldProtectionFlags));

        return true;
    }

    template <typename T>
    inline bool Write(void* in_pAddress, const std::vector<T>& in_rData)
    {
        if (!in_pAddress)
            return false;

        const auto length = sizeof(T) * in_rData.size();

        uint32_t oldProtectionFlags{};
        
        ASSERT_RETURN_FALSE(Protect(in_pAddress, length, GetProtectionFlags(PageProtection::RW), &oldProtectionFlags));
        ASSERT_RETURN_FALSE(memcpy_s(in_pAddress, length, in_rData.data(), length) == 0);
        ASSERT_RETURN_FALSE(Protect(in_pAddress, length, oldProtectionFlags));

        return true;
    }

    template <hedgedev::csl::ut::expr::AnyString T>
    inline bool WriteString(void* in_pAddress, const T& in_rStr)
    {
        if (!in_pAddress)
            return false;

        using TChar = hedgedev::csl::ut::expr::GetCharType_t<T>;

        return WriteStringFixedLength(in_pAddress, in_rStr, std::basic_string_view<TChar>(in_rStr).size());
    }

    template <hedgedev::csl::ut::expr::AnyString T>
    inline bool WriteStringFixedLength(void* in_pAddress, const T& in_rStr, size_t in_length)
    {
        if (!in_pAddress)
            return false;

        using TChar = hedgedev::csl::ut::expr::GetCharType_t<T>;

        auto view = std::basic_string_view<TChar>(in_rStr);

        auto srcLength = in_length;
        auto dstLength = srcLength;

        if (dstLength <= 0)
        {
            // Started at null terminator, abort.
            if (!*(TChar*)in_pAddress)
                return false;

            dstLength = std::basic_string_view<TChar>((const TChar*)in_pAddress).size() * sizeof(TChar);

            if (!dstLength)
                return false;

            srcLength = view.size() * sizeof(TChar);

            if (!srcLength)
                return false;

            // Use the smallest length to fit
            // within existing string boundaries.
            srcLength = std::min(srcLength, dstLength);
        }

        uint32_t oldProtectionFlags{};

        ASSERT_RETURN_FALSE(Protect(in_pAddress, dstLength, GetProtectionFlags(PageProtection::RW), &oldProtectionFlags));
        ASSERT_RETURN_FALSE(memcpy_s(in_pAddress, dstLength, view.data(), srcLength) == 0);
        memset((char*)(size_t(in_pAddress) + srcLength), 0, sizeof(TChar));
        ASSERT_RETURN_FALSE(Protect(in_pAddress, dstLength, oldProtectionFlags));

        return true;
    }
}
