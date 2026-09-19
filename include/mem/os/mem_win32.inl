#include "os/win32.h"
#include "ut/expr/expr.h"

namespace hedgedev::csl::mem
{
    inline static const HMODULE k_module = GetModuleHandle(NULL);
    inline static void* g_original_module_base{};
    
    inline void* get_original_module_base()
    {
        if (!k_module)
            return nullptr;

        const auto module_base = reinterpret_cast<uint8_t*>(k_module);
        const auto dos_header = reinterpret_cast<IMAGE_DOS_HEADER const*>(module_base);
        const auto nt_headers = reinterpret_cast<IMAGE_NT_HEADERS*>(module_base + dos_header->e_lfanew);

        return reinterpret_cast<void*>(nt_headers->OptionalHeader.ImageBase);
    }
    
    inline uint32_t get_protect_flags(page_protection in_protection)
    {
        uint32_t result = PAGE_NOACCESS;

        if (in_protection == page_protection::no_access)
            return result;

        uint32_t shift{};

        const auto has_read = ut::expr::has_flag(in_protection, page_protection::read);
        const auto has_write = ut::expr::has_flag(in_protection, page_protection::write);
        
        if (ut::expr::has_flag(in_protection, page_protection::execute))
        {
            result = PAGE_EXECUTE;
            shift = 4;
        }

        if (has_read && has_write)
        {
            result = PAGE_READWRITE << shift;
        }
        else if (has_read)
        {
            result = PAGE_READONLY << shift;
        }

        return result;
    }

    inline bool protect(void* in_address, size_t in_length, uint32_t in_new_protect_flags, uint32_t* out_old_protect_flags)
    {
        uint32_t old_protect_flags{};

        const auto result = VirtualProtect(in_address, in_length, in_new_protect_flags, reinterpret_cast<PDWORD>(&old_protect_flags));

        if (out_old_protect_flags)
            *out_old_protect_flags = old_protect_flags;

        return result;
    }

    inline void* to_aslr(void* in_address, void* in_base_address)
    {
        if (!in_base_address && !g_original_module_base)
            g_original_module_base = in_base_address = get_original_module_base();

        return reinterpret_cast<void*>(uintptr_t(k_module) + uintptr_t(in_address) - uintptr_t(in_base_address));
    }

    inline void* from_aslr(void* in_address, void* in_base_address)
    {
        if (!in_base_address && !g_original_module_base)
            g_original_module_base = in_base_address = get_original_module_base();
        
        return reinterpret_cast<void*>(uintptr_t(in_address) + uintptr_t(in_base_address) - uintptr_t(k_module));
    }
}
