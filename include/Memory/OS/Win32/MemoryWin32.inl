#pragma once

#include <string>

namespace hedgedev::csl::mem
{
    inline static const HMODULE g_khModule = GetModuleHandle(NULL);
    inline static void* g_pOrigModuleBase{};

    inline void* GetOriginalModuleBase()
    {
        if (!g_khModule)
            return nullptr;

        const auto pModuleBase = (uint8_t*)g_khModule;
        const auto pDosHeader = (IMAGE_DOS_HEADER*)pModuleBase;

        if (pDosHeader->e_magic != IMAGE_DOS_SIGNATURE)
            return nullptr;

        const auto pNtHeaders = (IMAGE_NT_HEADERS*)(pModuleBase + pDosHeader->e_lfanew);

        if (pNtHeaders->Signature != IMAGE_NT_SIGNATURE)
            return nullptr;

        return (void*)pNtHeaders->OptionalHeader.ImageBase;
    }

    inline void* ToASLR(void* in_pAddress, void* in_pBaseAddress)
    {
        if (!in_pBaseAddress && !g_pOrigModuleBase)
            g_pOrigModuleBase = in_pBaseAddress = GetOriginalModuleBase();

        return (void*)(size_t(g_khModule) + size_t(in_pAddress) - size_t(in_pBaseAddress));
    }

    inline void* FromASLR(void* in_pAddress, void* in_pBaseAddress)
    {
        if (!in_pBaseAddress && !g_pOrigModuleBase)
            g_pOrigModuleBase = in_pBaseAddress = GetOriginalModuleBase();
        
        return (void*)(size_t(in_pAddress) + size_t(in_pBaseAddress) - size_t(g_khModule));
    }

    inline bool IsNop(void* in_pAddress)
    {
#if defined(_M_AMD64) || defined(_M_IX86)
        return in_pAddress && Read<uint8_t>(in_pAddress) == 0x90;
#else
        static_assert(false, "IsNop is not implemented for this architecture.");
#endif
    }

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
    inline void* ReadInstructionAddress(void* in_pAddress, size_t in_offset, size_t in_stride)
    {
        if (!in_pAddress)
            return nullptr;

#if defined(_M_AMD64) || defined(_M_IX86)
        return (void*)((size_t(in_pAddress) + *(T*)(size_t(in_pAddress) + in_offset)) + in_stride);
#else
        static_assert(false, "ReadInstructionAddress is not implemented for this architecture.");
#endif
    }

    inline void* ReadCall(void* in_pAddress)
    {
        return ReadInstructionAddress<int>(in_pAddress, 1, 5);
    }

    inline void* ReadJump(void* in_pAddress)
    {
        if (!in_pAddress)
            return nullptr;

#if defined(_M_AMD64) || defined(_M_IX86)
        void* result{};

        const auto opcode = Read<uint8_t>(in_pAddress);
        auto jmpType = -1;
        
        if ((opcode & 0xF0) == 0x70)
        {
            jmpType = 0;
        }
        else
        {
            switch (opcode)
            {
                case 0xE3:
                case 0xEB:
                    jmpType = 0;
                    break;

                case 0xE9:
                    jmpType = 1;
                    break;

                case 0x0F:
                    jmpType = 2;
                    break;

                case 0xFF:
                    jmpType = 3;
                    break;
            }
        }
        
        switch (jmpType)
        {
            case 0:
                result = ReadInstructionAddress<int8_t>(in_pAddress, 1, 2);
                break;

            case 1:
                result = ReadInstructionAddress<int>(in_pAddress, 1, 5);
                break;

            case 2:
                result = ReadInstructionAddress<int>(in_pAddress, 2, 6);
                break;

            case 3:
                result = ReadInstructionAddress<int64_t>(in_pAddress, 6, 0);
                break;
        }
        
        return result;
#else
        static_assert(false, "ReadJump is not implemented for this architecture.");
#endif
    }

    template <typename T>
    inline bool Write(void* in_pAddress, const T& in_rData)
    {
        if (!in_pAddress)
            return false;

        DWORD oldProtect{};

        ASSERT_RETURN_FALSE(VirtualProtect(in_pAddress, sizeof(in_rData), PAGE_EXECUTE_READWRITE, &oldProtect));
        ASSERT_RETURN_FALSE(memcpy_s(in_pAddress, sizeof(in_rData), &in_rData, sizeof(in_rData)) == 0);
        ASSERT_RETURN_FALSE(VirtualProtect(in_pAddress, sizeof(in_rData), oldProtect, &oldProtect));

        return true;
    }

    template <typename T>
    inline bool Write(void* in_pAddress, const std::vector<T>& in_rData)
    {
        if (!in_pAddress)
            return false;

        const auto length = sizeof(T) * in_rData.size();
        DWORD oldProtect{};

        ASSERT_RETURN_FALSE(VirtualProtect(in_pAddress, length, PAGE_EXECUTE_READWRITE, &oldProtect));
        ASSERT_RETURN_FALSE(memcpy_s(in_pAddress, length, in_rData.data(), length) == 0);
        ASSERT_RETURN_FALSE(VirtualProtect(in_pAddress, length, oldProtect, &oldProtect));

        return true;
    }

    inline bool WriteCall(void* in_pAddress, void* in_pDestination)
    {
        return WriteJump(in_pAddress, in_pDestination, true);
    }

    inline bool WriteJump(void* in_pAddress, void* in_pDestination, bool in_isCall)
    {
        if (!in_pAddress)
            return false;

#if defined(_M_AMD64) || defined(_M_IX86)
        auto length = size_t(in_pDestination) - size_t(in_pAddress);

        if (length - 2 <= 0x7F && !in_isCall)
        {
            ASSERT_RETURN_FALSE(Write<uint8_t>(in_pAddress, 0xEB));
            ASSERT_RETURN_FALSE(Write<uint8_t>(((uint8_t*)in_pAddress) + 1, uint8_t(length - 2)));
        }
        else
        {
            if (length - 5 <= 0x7FFFFFFF)
            {
                ASSERT_RETURN_FALSE(Write<uint8_t>(in_pAddress, in_isCall ? 0xE8 : 0xE9));
                ASSERT_RETURN_FALSE(Write<uint32_t>(((uint8_t*)in_pAddress) + 1, uint32_t(length - 5)));
            }
#ifdef WIN64
            else
            {
                ASSERT_RETURN_FALSE(Write<uint8_t>(in_pAddress, 0xFF));
                ASSERT_RETURN_FALSE(Write<uint8_t>(((uint8_t*)in_pAddress) + 1, in_isCall ? 0x15 : 0x25));
                ASSERT_RETURN_FALSE(Write<uint32_t>(((uint8_t*)in_pAddress) + 2, 0));
                ASSERT_RETURN_FALSE(Write<uint64_t>(((uint8_t*)in_pAddress) + 6, uint64_t(length - 14)));
            }
#endif // WIN64
        }

        return true;
#else
        static_assert(false, "WriteJump is not implemented for this architecture.");
#endif
    }

    inline bool WriteNop(void* in_pAddress, size_t in_length)
    {
        if (!in_pAddress)
            return false;
        
        DWORD oldProtect{};

        ASSERT_RETURN_FALSE(VirtualProtect(in_pAddress, in_length, PAGE_EXECUTE_READWRITE, &oldProtect));

#if defined(_M_AMD64) || defined(_M_IX86)
        memset((char*)in_pAddress, 0x90, in_length);
#else
        static_assert(false, "WriteNop is not implemented for this architecture.");
#endif

        ASSERT_RETURN_FALSE(VirtualProtect(in_pAddress, in_length, oldProtect, &oldProtect));

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

        DWORD oldProtect{};

        ASSERT_RETURN_FALSE(VirtualProtect(in_pAddress, dstLength, PAGE_EXECUTE_READWRITE, &oldProtect));
        ASSERT_RETURN_FALSE(memcpy_s(in_pAddress, dstLength, view.data(), srcLength) == 0);
        memset((void*)(size_t(in_pAddress) + srcLength), 0, sizeof(TChar));
        ASSERT_RETURN_FALSE(VirtualProtect(in_pAddress, dstLength, oldProtect, &oldProtect));

        return true;
    }
}
