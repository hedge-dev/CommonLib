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
        const auto pNtHeaders = (IMAGE_NT_HEADERS*)(pModuleBase + pDosHeader->e_lfanew);

        return (void*)pNtHeaders->OptionalHeader.ImageBase;
    }
    
    inline uint32_t GetProtectionFlags(PageProtection in_protection)
    {
        if (in_protection == PageProtection::NoAccess)
            return PAGE_NOACCESS;

        uint32_t result{};
        uint32_t shift{};

        const auto hasRead = hedgedev::csl::ut::expr::HasFlag(in_protection, PageProtection::Read);
        const auto hasWrite = hedgedev::csl::ut::expr::HasFlag(in_protection, PageProtection::Write);
        
        if (hedgedev::csl::ut::expr::HasFlag(in_protection, PageProtection::Execute))
        {
            result = PAGE_EXECUTE;
            shift = 4;
        }

        if (hasRead && hasWrite)
        {
            result = PAGE_READWRITE << shift;
        }
        else if (hasRead)
        {
            result = PAGE_READONLY << shift;
        }

        return result;
    }

    inline bool Protect(void* in_pAddress, size_t in_length, uint32_t in_newProtectionFlags, uint32_t* out_pOldProtectionFlags)
    {
        return VirtualProtect(in_pAddress, in_length, in_newProtectionFlags, (PDWORD)out_pOldProtectionFlags);
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
}
