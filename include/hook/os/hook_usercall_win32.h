#pragma once

#include <bit>
#include <functional>
#include <optional>
#include <typeindex>
#include <type_traits>
#include <vector>

#include "../thirdparty/Detours/src/detours.h"
#include "mem/mem.h"
#include "os/win32.h"

#define __CMNLIB_INTERNAL_USER_SIZE_OF(RETURN_TYPE) \
    sizeof(typename std::conditional_t<std::is_void_v<RETURN_TYPE>, RETURN_TYPE*, RETURN_TYPE>)

///
/// A custom calling convention that stores arguments in optimised locations,
/// before falling back to using the stack, similar to `__fastcall`.
///
/// This calling convention puts the responsibility of cleaning up stack
/// arguments on the caller.
///
/// This identifier cannot be used as a standard calling convention.
///
#define __usercall 0

///
/// A custom calling convention that stores arguments in optimised locations,
/// before falling back to using the stack, similar to `__fastcall`.
///
/// This calling convention puts the responsibility of cleaning up stack
/// arguments on the callee.
///
/// This identifier cannot be used as a standard calling convention.
///
#define __userpurge 1

#define USER_REGISTER(REGISTER) \
    uint64_t(hedgedev::csl::hook::user_register(hedgedev::csl::hook::user_register_alias::REGISTER))

///
/// Specify the register to use for the return value.
///
/// \param REGISTER The register to use for the return value.
///
#define USER_RETURN(REGISTER) \
    USER_REGISTER(REGISTER)

///
/// Specifies the hook has no return value.
///
#define USER_RETURN_VOID \
    USER_RETURN(none)

///
/// Specify the register to use for a specific parameter.
///
/// \param INDEX    The index of the parameter.
/// \param REGISTER The register to use for the parameter.
///
#define USER_PARAM(INDEX, REGISTER) \
    (USER_REGISTER(REGISTER) << ((INDEX + 1) * hedgedev::csl::hook::k_user_register_size))

///
/// \class __CMNLIB_INTERNAL_USER_HOOK_COMMON_PARAMS
///
/// \param RETURN_TYPE        The return type of the function.
/// \param CALLING_CONVENTION The calling convention of the function, such as
///                           `__usercall` or `__userpurge`.
/// \param FUNCTION_NAME      The name of the function.
/// \param ADDRESS            The address of the function.
/// \param REGISTERS          The registers used by the return value and parameters.
///                           If a parameter has an unspecified register, it'll
///                           automatically be put on the stack.
/// \param PARAM_COUNT        The total number of parameters in `__VA_ARGS__`.
/// \param __VA_ARGS__        The parameters of the function.
///

///
/// \class __CMNLIB_INTERNAL_USER_HOOK_COMMON_PROTOTYPE
///
/// For an optimised function with the following prototype:
/// \code{.cpp}
/// int __usercall my_optimised_function@<eax>(int in_first_param@<eax>, float in_second_param@<xmm0>, int in_third_param)
/// \endcode
///

///
/// Declares a pointer to a function with custom calling convention in memory.
///
/// \copydoc __CMNLIB_INTERNAL_USER_HOOK_COMMON_PARAMS
///
/// ## Examples
/// \copydoc __CMNLIB_INTERNAL_USER_HOOK_COMMON_PROTOTYPE
///
/// A function pointer should be set up like the following:
/// \code{.cpp}
/// USER_FUNCTION_PTR(int, __usercall, my_optimised_function, 0xDEADBEEF, USER_RETURN(EAX) | USER_PARAM(0, EAX) | USER_PARAM(1, XMM0), 3, int in_first_param, float in_second_param, int in_third_param);
/// \endcode
///
#define USER_FUNCTION_PTR(RETURN_TYPE, CALLING_CONVENTION, FUNCTION_NAME, ADDRESS, REGISTERS, PARAM_COUNT, ...)                                                                                   \
    hedgedev::csl::hook::usercall_info g_info_##FUNCTION_NAME##_fp                                                                                                                                \
        { typeid(RETURN_TYPE), __CMNLIB_INTERNAL_USER_SIZE_OF(RETURN_TYPE), CALLING_CONVENTION, reinterpret_cast<void*>(ADDRESS), reinterpret_cast<void*>(ADDRESS), REGISTERS, PARAM_COUNT };     \
    void* g_trampoline_to_##FUNCTION_NAME = hedgedev::csl::hook::emit_user_trampoline(g_info_##FUNCTION_NAME##_fp, reinterpret_cast<void*>(&g_info_##FUNCTION_NAME##_fp.original_address), true); \
    FUNCTION_PTR(RETURN_TYPE, __cdecl, FUNCTION_NAME, g_trampoline_to_##FUNCTION_NAME, __VA_ARGS__)

///
/// \class __CMNLIB_INTERNAL_USER_HOOK_COMMON
///
/// \copydoc __CMNLIB_INTERNAL_USER_HOOK_COMMON_PARAMS
///
/// ## Examples
/// \copydoc __CMNLIB_INTERNAL_USER_HOOK_COMMON_PROTOTYPE
///
/// A hook should be set up like the following:
/// \code{.cpp}
/// USER_HOOK(int, __usercall, my_optimised_function_hook, 0xDEADBEEF, USER_RETURN(EAX) | USER_PARAM(0, EAX) | USER_PARAM(1, XMM0), 3, int in_first_param, float in_second_param, int in_third_param)
/// {
///     return original_my_optimised_function_hook(in_first_param, in_second_param, in_third_param);
/// }
/// \endcode
///

///
/// Defines the body of a hook for a function with custom calling convention
/// in memory.
///
/// \copydoc __CMNLIB_INTERNAL_USER_HOOK_COMMON
///
#define USER_HOOK(RETURN_TYPE, CALLING_CONVENTION, FUNCTION_NAME, ADDRESS, REGISTERS, PARAM_COUNT, ...)                                                                                       \
    hedgedev::csl::hook::usercall_info g_info_##FUNCTION_NAME                                                                                                                                 \
        { typeid(RETURN_TYPE), __CMNLIB_INTERNAL_USER_SIZE_OF(RETURN_TYPE), CALLING_CONVENTION, reinterpret_cast<void*>(ADDRESS), reinterpret_cast<void*>(ADDRESS), REGISTERS, PARAM_COUNT }; \
    RETURN_TYPE __cdecl impl_##FUNCTION_NAME(__VA_ARGS__);                                                                                                                                    \
    void* g_trampoline_to_##FUNCTION_NAME##_hook = hedgedev::csl::hook::emit_user_trampoline(g_info_##FUNCTION_NAME, reinterpret_cast<void*>(&impl_##FUNCTION_NAME));                         \
    HOOK(RETURN_TYPE, __cdecl, FUNCTION_NAME, nullptr, __VA_ARGS__)

///
/// Defines the body of a hook for a function with custom calling convention
/// in memory, and installs it upon initialisation.
///
/// \copydoc __CMNLIB_INTERNAL_USER_HOOK_COMMON
///
/// \returns Use \ref GET_STATIC_HOOK_RESULT.
///
#define STATIC_USER_HOOK(RETURN_TYPE, CALLING_CONVENTION, FUNCTION_NAME, ADDRESS, REGISTERS, PARAM_COUNT, ...) \
    USER_HOOK(RETURN_TYPE, CALLING_CONVENTION, FUNCTION_NAME, ADDRESS, REGISTERS, PARAM_COUNT, __VA_ARGS__);   \
    __CMNLIB_INTERNAL_STATIC_HOOK_IMPL(FUNCTION_NAME, ADDRESS, INSTALL_USER_HOOK)                              \
    RETURN_TYPE __cdecl impl_##FUNCTION_NAME(__VA_ARGS__)

///
/// Installs a hook defined with \ref USER_HOOK.
///
/// \param FUNCTION_NAME The name of the function to call before the original.
///
/// \returns `true` if the installation succeeeded, or if the hook was already
///          installed. Otherwise, `false`.
///
#define INSTALL_USER_HOOK(FUNCTION_NAME) \
    INSTALL_USER_HOOK_EXPLICIT(FUNCTION_NAME, g_info_##FUNCTION_NAME.detour_address)

///
/// Installs a hook defined with \ref USER_HOOK at an explicit address.
///
/// \param FUNCTION_NAME The name of the function to call before the original.
/// \param ADDRESS       The address of the function to hook.
///
/// \returns `true` if the installation succeeeded, or if the hook was already
///          installed. Otherwise, `false`.
///
#define INSTALL_USER_HOOK_EXPLICIT(FUNCTION_NAME, ADDRESS)                                                            \
    std::invoke([&]() -> bool                                                                                         \
    {                                                                                                                 \
        auto& info = g_info_##FUNCTION_NAME;                                                                          \
                                                                                                                      \
        if (!info.detour_address && !(ADDRESS))                                                                       \
            return false;                                                                                             \
                                                                                                                      \
        *reinterpret_cast<void**>(&info.detour_address) = reinterpret_cast<void*>(ADDRESS);                           \
                                                                                                                      \
        DetourTransactionBegin();                                                                                     \
        DetourUpdateThread(GetCurrentThread());                                                                       \
        DetourAttach(reinterpret_cast<void**>(&info.detour_address), g_trampoline_to_##FUNCTION_NAME##_hook);         \
                                                                                                                      \
        const auto result = DetourTransactionCommit() == NO_ERROR;                                                    \
                                                                                                                      \
        if (result)                                                                                                   \
        {                                                                                                             \
            *reinterpret_cast<void**>(&original_##FUNCTION_NAME) =                                                    \
                hedgedev::csl::hook::emit_user_trampoline(info, reinterpret_cast<void*>(&info.detour_address), true); \
                                                                                                                      \
            g_address_post_hook_##FUNCTION_NAME =                                                                     \
                hedgedev::csl::hook::_get_post_hook_address(reinterpret_cast<void*>(g_address_##FUNCTION_NAME));      \
        }                                                                                                             \
                                                                                                                      \
        return result;                                                                                                \
    })

///
/// Uninstalls a hook installed with \ref INSTALL_USER_HOOK.
///
/// \param FUNCTION_NAME The name of the function to unhook.
///
/// \returns `true` if the uninstallation succeeeded, or if the hook was already
///          uninstalled. Otherwise, `false`.
///
#define UNINSTALL_USER_HOOK(FUNCTION_NAME)                                                                    \
    std::invoke([&]() -> bool                                                                                 \
    {                                                                                                         \
        auto& info = g_info_##FUNCTION_NAME;                                                                  \
                                                                                                              \
        if (info.original_address == info.detour_address)                                                     \
            return true;                                                                                      \
                                                                                                              \
        DetourTransactionBegin();                                                                             \
        DetourUpdateThread(GetCurrentThread());                                                               \
        DetourDetach(reinterpret_cast<void**>(&info.detour_address), g_trampoline_to_##FUNCTION_NAME##_hook); \
                                                                                                              \
        return DetourTransactionCommit() == NO_ERROR;                                                         \
    })

namespace hedgedev::csl::hook
{
    ///
    /// Registers used for `__usercall`/`__userpurge` calling conventions.
    ///
    /// These registers have been ordered in an specific way that line up with
    /// register IDs used for encoding instructions.
    ///
    enum class user_register : uint64_t
    {
        none,

        EAX,
        ECX,
        EDX,
        EBX,
        ESP,
        EBP,
        ESI,
        EDI,

        ST0,
        ST1,
        ST2,
        ST3,
        ST4,
        ST5,
        ST6,
        ST7,

        XMM0,
        XMM1,
        XMM2,
        XMM3,
        XMM4,
        XMM5,
        XMM6,
        XMM7,

        count
    };

    enum class user_register_alias : uint64_t
    {
        none = user_register::none,
        
        AL  = user_register::EAX,
        AX  = user_register::EAX,
        EAX = user_register::EAX,

        CL  = user_register::ECX,
        CX  = user_register::ECX,
        ECX = user_register::ECX,

        DL  = user_register::EDX,
        DX  = user_register::EDX,
        EDX = user_register::EDX,

        BL  = user_register::EBX,
        BX  = user_register::EBX,
        EBX = user_register::EBX,

        SPL = user_register::ESP,
        SP  = user_register::ESP,
        ESP = user_register::ESP,

        BPL = user_register::EBP,
        BP  = user_register::EBP,
        EBP = user_register::EBP,
        
        SIL = user_register::ESI,
        SI  = user_register::ESI,
        ESI = user_register::ESI,

        DIL = user_register::EDI,
        DI  = user_register::EDI,
        EDI = user_register::EDI,

        ST0 = user_register::ST0,
        ST1 = user_register::ST1,
        ST2 = user_register::ST2,
        ST3 = user_register::ST3,
        ST4 = user_register::ST4,
        ST5 = user_register::ST5,
        ST6 = user_register::ST6,
        ST7 = user_register::ST7,
        
        XMM0 = user_register::XMM0,
        XMM1 = user_register::XMM1,
        XMM2 = user_register::XMM2,
        XMM3 = user_register::XMM3,
        XMM4 = user_register::XMM4,
        XMM5 = user_register::XMM5,
        XMM6 = user_register::XMM6,
        XMM7 = user_register::XMM7
    };

    enum class user_register_family : uint64_t
    {
        none,

        GPR = user_register::EAX,
        FPU = user_register::ST0,
        SSE = user_register::XMM0
    };

    ///
    /// The total number of bits in the register flags.
    ///
    inline static constexpr size_t k_user_register_bits_length = sizeof(uint64_t) * sizeof(uint64_t);

    ///
    /// The total number of bits per register in the register flags.
    ///
    inline static constexpr size_t k_user_register_size = std::bit_width(uint64_t(user_register::count));

    ///
    /// The maximum number of registers that can fit in the register flags.
    ///
    inline static constexpr size_t k_user_register_max = k_user_register_bits_length / k_user_register_size;

    ///
    /// The number of remaining bits when all slots are used up in the register flags.
    ///
    inline static constexpr size_t k_user_register_bits_remainder = k_user_register_bits_length % k_user_register_size;

    ///
    /// The mask for getting the register bits.
    ///
    inline static constexpr uint64_t k_user_register_mask = 0x1F;

    struct usercall_info
    {
        std::type_index return_type;
        size_t return_type_size{};
        bool is_userpurge{};
        void* original_address{};
        void* detour_address{};
        uint64_t registers{};
        uint8_t param_count{};

        size_t get_register_count() const
        {
            return size_t(std::bit_width(registers) + k_user_register_bits_remainder) / k_user_register_size;
        }

        size_t get_register_param_count() const
        {
            const auto register_count = get_register_count();

            if (register_count <= 1)
                return 0;

            return register_count - 1;
        }

        size_t get_stack_param_count() const
        {
            return param_count - get_register_param_count();
        }

        size_t get_param_count() const
        {
            return get_register_param_count() + get_stack_param_count();
        }

        user_register get_register(int in_index) const
        {
            return user_register(registers >> (k_user_register_size * in_index) & k_user_register_mask);
        }

        user_register get_return_register() const
        {
            return get_register(0);
        }
        
        user_register get_param_register(int in_index) const
        {
            return get_register(in_index + 1);
        }

        template <typename T>
        bool is_return_type() const
        {
            return typeid(T) == return_type;
        }

        bool is_stack_param(int in_index) const
        {
            return get_register_param_count() < size_t(in_index + 1);
        }
    };

    ///
    /// Gets the family a register belongs to.
    ///
    /// \param in_register The register to check.
    ///
    inline user_register_family get_register_family(user_register in_register)
    {
        if (in_register >= user_register::EAX && in_register <= user_register::EDI)
        {
            return user_register_family::GPR;
        }
        else if (in_register >= user_register::ST0 && in_register <= user_register::ST7)
        {
            return user_register_family::FPU;
        }
        else if (in_register >= user_register::XMM0 && in_register <= user_register::XMM7)
        {
            return user_register_family::SSE;
        }

        return user_register_family::none;
    }

    ///
    /// Gets the ID of a register.
    ///
    /// \param in_register The register to get the ID for.
    ///
    inline int get_register_id(user_register in_register)
    {
        return int(in_register) - int(get_register_family(in_register));
    }

    ///
    /// Checks if a register is protected and should be restored after calling the
    /// original function.
    ///
    /// \param in_register The register to check.
    ///
    inline bool is_protected_register(user_register in_register)
    {
        return in_register == user_register::EBX ||
               in_register == user_register::EBP ||
               in_register == user_register::ESI ||
               in_register == user_register::EDI;
    }

    ///
    /// Emits a trampoline that forwards arguments from a function with custom calling
    /// convention to a `__cdecl` function in a hook defined with \ref USER_HOOK.
    ///
    /// \param in_info        The info about the hook.
    /// \param in_address     The address of the `__cdecl` function to trampoline to.
    /// \param in_to_original Determines whether this trampoline is going back to the
    ///                       original function.
    ///
    /// \returns A pointer to the trampoline that was emitted.
    ///
    inline void* emit_user_trampoline(const usercall_info& in_info, void* in_address, bool in_to_original = false)
    {
        std::vector<uint8_t> result{};

        const auto emit_imm8 = [&](int in_value) -> size_t
        {
            auto pos = result.size();

            result.push_back(in_value & 0xFF);

            return pos;
        };
        
        const auto emit_imm16 = [&](int in_value) -> size_t
        {
            auto pos = emit_imm8(in_value);

            result.push_back((in_value >> 8) & 0xFF);

            return pos;
        };

        const auto emit_imm32 = [&](int in_value) -> size_t
        {
            auto pos = emit_imm16(in_value);

            result.push_back((in_value >> 16) & 0xFF);
            result.push_back((in_value >> 24) & 0xFF);

            return pos;
        };

        const auto emit_push = [&](user_register in_register, std::optional<int> in_offset = {})
        {
            const auto register_id = get_register_id(in_register);

            if (in_offset.has_value())
            {
                const auto offset = in_offset.value();
                const auto emit_ebp_no_offset = in_register == user_register::EBP && offset == 0;

                result.push_back(0xFF);

                if (!offset && !emit_ebp_no_offset)
                {
                    // push [{register}]
                    result.push_back(uint8_t(0x30 | register_id));
                }
                else if (offset <= 0x7F || emit_ebp_no_offset)
                {
                    // push [{register} + {imm8}]
                    result.push_back(uint8_t(0x70 | register_id));
                }
                else
                {
                    // push [{register} + {imm32}]
                    result.push_back(uint8_t(0xB0 | register_id));
                }

                if (in_register == user_register::ESP)
                    result.push_back(0x24);

                if (!offset && !emit_ebp_no_offset)
                    return;
                
                if ((offset > 0 && offset <= 0x7F) || emit_ebp_no_offset)
                {
                    emit_imm8(offset);
                }
                else if (offset > 0x7F)
                {
                    emit_imm32(offset);
                }

                return;
            }

            // push {register}
            result.push_back(uint8_t(0x50 | register_id));
        };

        const auto emit_pop = [&](user_register in_register)
        {
            // pop {register}
            result.push_back(uint8_t(0x58 | get_register_id(in_register)));
        };

        const auto emit_add_subtract = [&](user_register in_register, int in_value)
        {
            if (!in_value)
                return;
            
            const auto register_id = get_register_id(in_register);
            const auto is_add = in_value > 0;
            const auto value_abs = std::abs(in_value);

            if (value_abs == 1)
            {
                auto opcode = 0x40;

                if (!is_add)
                    opcode |= 8;

                // inc/dec {register}
                result.push_back(uint8_t(opcode | register_id));
            }
            else
            {
                auto modifier = 0;

                if (!is_add)
                    modifier = 0x28;

                if (value_abs > 0 && value_abs <= 0x7F)
                {
                    // add/sub {register}, {imm8}
                    result.push_back(0x83);
                    result.push_back(uint8_t(0xC0 | modifier | register_id));
                    emit_imm8(value_abs);
                }
                else if (value_abs > 0x7F)
                {
                    // add/sub {register}, {imm32}
                    if (in_register == user_register::EAX)
                    {
                        result.push_back(uint8_t(0x05 | modifier));
                    }
                    else
                    {
                        result.push_back(0x81);
                        result.push_back(uint8_t(0xC0 | modifier | register_id));
                    }

                    emit_imm32(value_abs);
                }
            }
        };

        const auto emit_load_store_sse = [&](user_register in_dst, user_register in_src, int in_offset = 0, int in_float_size = sizeof(float), bool in_store = false)
        {
            const auto dst_register_id = get_register_id(in_dst);
            const auto src_register_id = get_register_id(in_src);

            const auto ptr_register = in_store ? in_dst : in_src;
            const auto ptr_register_id = in_store ? dst_register_id : src_register_id;

            const auto data_register = in_store ? in_src : in_dst;
            const auto data_register_id = in_store ? src_register_id : dst_register_id;

            const auto emit_ebp_no_offset = ptr_register == user_register::EBP && in_offset == 0;

            // movss dword ptr
            auto opcode = 0xF3;

            // movsd qword ptr
            if (in_float_size == sizeof(double))
                opcode = 0xF2;

            result.push_back(opcode);
            result.push_back(0x0F);
            result.push_back(in_store ? 0x11 : 0x10);

            auto modifier = 0;

            if ((in_offset > 0 && in_offset <= 0x7F) || emit_ebp_no_offset)
            {
                modifier = 0x40;
            }
            else if (in_offset > 0x7F)
            {
                modifier = 0x80;
            }
            
            if (ptr_register == user_register::ESP)
            {
                result.push_back(uint8_t(0x04 | modifier | (data_register_id << 3)));
                result.push_back(0x24);
            }
            else
            {
                result.push_back(uint8_t(modifier | (ptr_register_id << 3) | data_register_id));
            }

            if ((in_offset > 0 && in_offset <= 0x7F) || emit_ebp_no_offset)
            {
                // movss/movsd {dst}, dword/qword ptr [{src} + {imm8}]
                // movss/movsd dword/qword ptr [{dst} + {imm8}], {src}
                emit_imm8(in_offset);
            }
            else if (in_offset > 0x7F)
            {
                // movss/movsd {dst}, dword/qword ptr [{src} + {imm32}]
                // movss/movsd dword/qword ptr [{dst} + {imm32}], {src}
                emit_imm32(in_offset);
            }
        };

        const auto emit_exchange_register_fpu = [&](user_register in_register)
        {
            if (get_register_family(in_register) != user_register_family::FPU)
                return;
            
            // fxch {register}
            result.push_back(0xD9);
            result.push_back(uint8_t(0xC8 | get_register_id(in_register)));
        };

        const auto emit_load_store_fpu = [&](user_register in_register, int in_offset = 0, int in_float_size = sizeof(float), bool in_store = false)
        {
            const auto registerId = get_register_id(in_register);

            switch (get_register_family(in_register))
            {
                case user_register_family::GPR:
                {
                    const auto emit_ebp_no_offset = in_register == user_register::EBP && in_offset == 0;

                    // fld dword ptr
                    auto opcode = 0xD9;

                    // fld qword ptr
                    if (in_float_size == sizeof(double))
                        opcode |= 4;
                    
                    result.push_back(opcode);
                    
                    auto modifier = 0;

                    if ((in_offset > 0 && in_offset <= 0x7F) || emit_ebp_no_offset)
                    {
                        modifier = 0x40;
                    }
                    else if (in_offset > 0x7F)
                    {
                        modifier = 0x80;
                    }

                    // fstp
                    if (in_store)
                        modifier |= 0x18;
                    
                    if (in_register == user_register::ESP)
                        modifier |= 4;

                    result.push_back(uint8_t(modifier | registerId));
                    
                    if (in_register == user_register::ESP)
                        result.push_back(0x24);

                    if ((in_offset > 0 && in_offset <= 0x7F) || emit_ebp_no_offset)
                    {
                        // fld/fstp dword/qword ptr [{register} + {imm8}]
                        emit_imm8(in_offset);
                    }
                    else if (in_offset > 0x7F)
                    {
                        // fld/fstp dword/qword ptr [{register} + {imm32}]
                        emit_imm32(in_offset);
                    }

                    break;
                }

                // fstp {dst}
                case user_register_family::FPU:
                    result.push_back(0xDD);
                    result.push_back(uint8_t(0xD8 | registerId));
                    break;
            }
        };

        const auto emit_load_store = [&](user_register in_dst, user_register in_src, int in_offset = 0, bool in_store = false)
        {
            const auto dst_register_id = get_register_id(in_dst);
            const auto src_register_id = get_register_id(in_src);

            const auto ptr_register = in_store ? in_dst : in_src;
            const auto ptr_register_id = in_store ? dst_register_id : src_register_id;

            const auto data_register = in_store ? in_src : in_dst;
            const auto data_register_id = in_store ? src_register_id : dst_register_id;

            const auto emit_ebp_no_offset = ptr_register == user_register::EBP && in_offset == 0;

            // mov [{dst}], {src}
            auto opcode = 0x89;

            // mov {dst}, [{src}]
            if (!in_store)
                opcode |= 2;

            result.push_back(opcode);
                    
            auto modifier = 0;

            if ((in_offset > 0 && in_offset <= 0x7F) || emit_ebp_no_offset)
            {
                modifier = 0x40;
            }
            else if (in_offset > 0x7F)
            {
                modifier = 0x80;
            }
            
            if (ptr_register == user_register::ESP)
            {
                result.push_back(uint8_t(0x04 | modifier | (data_register_id << 3)));
                result.push_back(0x24);
            }
            else
            {
                result.push_back(uint8_t(modifier | (ptr_register_id << 3) | data_register_id));
            }

            if ((in_offset > 0 && in_offset <= 0x7F) || emit_ebp_no_offset)
            {
                // mov {dst}, [{src} + {imm8}]
                // mov [{dst} + {imm8}], {src}
                emit_imm8(in_offset);
            }
            else if (in_offset > 0x7F)
            {
                // mov {dst}, [{src} + {imm32}]
                // mov [{dst} + {imm32}], {src}
                emit_imm32(in_offset);
            }
        };

        const auto emit_move_register_fpu = [&](user_register in_dst, user_register in_src, int in_float_size = sizeof(float))
        {
            const auto dst_register_family = get_register_family(in_dst);
            const auto src_register_family = get_register_family(in_src);

            if (dst_register_family == user_register_family::FPU)
            {
                if (dst_register_family == src_register_family)
                {
                    if (in_dst == in_src)
                        return;
                    
                    // Exchange ST(0) with source register.
                    emit_exchange_register_fpu(in_src);

                    // Copy ST(0) to destination register.
                    emit_load_store_fpu(in_dst);

                    // Restore ST(0).
                    emit_exchange_register_fpu(in_src);
                }
                else
                {
                    // Allocate stack space for register.
                    emit_add_subtract(user_register::ESP, -in_float_size);

                    if (src_register_family == user_register_family::GPR)
                    {
                        // Copy GPR register into stack.
                        emit_load_store(user_register::ESP, in_src, 0, true);
                    }
                    else if (src_register_family == user_register_family::SSE)
                    {
                        // Copy SSE register into stack.
                        emit_load_store_sse(user_register::ESP, in_src, 0, in_float_size, true);
                    }
                    
                    // Exchange ST(0) with destination register.
                    if (in_dst != user_register::ST0)
                        emit_exchange_register_fpu(in_dst);

                    // Load stack value into FPU register.
                    emit_load_store_fpu(user_register::ESP, 0, in_float_size);

                    // Restore ST(0).
                    if (in_dst != user_register::ST0)
                        emit_exchange_register_fpu(in_dst);

                    // Deallocate stack space.
                    emit_add_subtract(user_register::ESP, in_float_size);
                }
            }
            else
            {
                // Allocate stack space for FPU register.
                emit_add_subtract(user_register::ESP, -in_float_size);

                // Exchange ST(0) with source register.
                if (in_src != user_register::ST0)
                    emit_exchange_register_fpu(in_src);

                // Copy ST(0) into stack.
                emit_load_store_fpu(user_register::ESP, 0, in_float_size, true);

                // Restore ST(0).
                if (in_src != user_register::ST0)
                    emit_exchange_register_fpu(in_src);

                if (dst_register_family == user_register_family::GPR)
                {
                    // Load stack value into GPR register.
                    emit_load_store(in_dst, user_register::ESP, 0);
                }
                else if (dst_register_family == user_register_family::SSE)
                {
                    // Load stack value into SSE register.
                    emit_load_store_sse(in_dst, user_register::ESP, 0, in_float_size);
                }

                // Deallocate stack space.
                emit_add_subtract(user_register::ESP, in_float_size);
            }
        };

        const auto emit_move_register_sse = [&](user_register in_dst, user_register in_src, int in_float_size = sizeof(float))
        {
            const auto dst_register_id = get_register_id(in_dst);
            const auto src_register_id = get_register_id(in_src);
            const auto dst_register_family = get_register_family(in_dst);
            const auto src_register_family = get_register_family(in_src);

            if (dst_register_family == src_register_family && dst_register_family == user_register_family::SSE)
            {
                // movss {dst}, {src}
                auto opcode = 0xF3;

                // movsd {dst}, {src}
                if (in_float_size == sizeof(double))
                    opcode = 0xF2;
                
                result.push_back(opcode);
                result.push_back(0x0F);
                result.push_back(0x10);
                result.push_back(uint8_t(0xC0 | (src_register_id << 3) | dst_register_id));
            }
            else if ((dst_register_family == user_register_family::GPR || dst_register_family == user_register_family::SSE) &&
                     (src_register_family == user_register_family::GPR || src_register_family == user_register_family::SSE))
            {
                auto modifier = 0x6E;

                if (dst_register_family == user_register_family::GPR)
                    modifier |= 0x10;

                // movd {dst}, {src}
                result.push_back(0x66);
                result.push_back(0x0F);
                result.push_back(modifier);
                result.push_back(uint8_t(0xC0 | (src_register_id << 3) | dst_register_id));
            }
            else
            {
                emit_move_register_fpu(in_dst, in_src, in_float_size);
            }
        };

        const auto emit_move_register = [&](user_register in_dst, user_register in_src, int in_float_size = sizeof(float))
        {
            const auto dst_register_family = get_register_family(in_dst);
            const auto src_register_family = get_register_family(in_src);

            if (dst_register_family == src_register_family)
            {
                switch (dst_register_family)
                {
                    // mov {dst}, {src}
                    case user_register_family::GPR:
                        result.push_back(0x89);
                        result.push_back(uint8_t(0xC0 | (get_register_id(in_src) << 3) | get_register_id(in_dst)));
                        break;

                    case user_register_family::FPU:
                        emit_move_register_fpu(in_dst, in_src, in_float_size);
                        break;

                    case user_register_family::SSE:
                        emit_move_register_sse(in_dst, in_src, in_float_size);
                        break;
                }
            }
            else
            {
                emit_move_register_sse(in_dst, in_src, in_float_size);
            }
        };

        const auto emit_branch = [&](bool in_call, bool in_far) -> size_t
        {
            if (in_far)
            {
                // jmp/call [{imm32}]
                result.push_back(0xFF);
                result.push_back(in_call ? 0x15 : 0x25);
            }
            else
            {
                // jmp/call {imm32}
                result.push_back(in_call ? 0xE8 : 0xE9);
            }

            return emit_imm32(0);
        };

        const auto emit_branch_address = [&](void* in_trampoline, ptrdiff_t in_branch_offset, void* in_target, bool in_absolute)
        {
            const auto offset = uintptr_t(in_trampoline) + in_branch_offset;

            if (in_absolute)
            {
                *reinterpret_cast<uint32_t*>(offset) = uint32_t(in_target);
            }
            else
            {
                *reinterpret_cast<uint32_t*>(offset) = uint32_t(uintptr_t(in_target) - offset - sizeof(uint32_t));
            }
        };

        const auto emit_return = [&](size_t in_size = 0)
        {
            auto opcode = 0xC2;

            if (!in_size)
                opcode |= 1;

            // ret
            result.push_back(opcode);

            if (!in_size)
                return;
            
            // ret {imm16}
            emit_imm16(in_size);
        };

        const auto register_count = in_info.get_register_count();
        const auto stack_param_count = in_info.get_stack_param_count();
        const auto param_count = in_info.get_param_count();
        
        auto stack_offset = in_to_original
            ? param_count * 4
            : stack_param_count * 4;

        if (in_to_original)
        {
            // Back up protected registers.
            for (size_t i = 0; i < register_count; i++)
            {
                const auto current_register = in_info.get_register(i);

                if (!is_protected_register(current_register))
                    continue;
                
                emit_push(current_register);
                stack_offset += 4;
            }
        }

        // Set up arguments.
        for (auto i = param_count; i-- > 0;)
        {
            if (in_info.is_stack_param(i))
            {
                emit_push(user_register::ESP, stack_offset);
            }
            else
            {
                const auto current_register = in_info.get_param_register(i);

                if (in_to_original)
                {
                    // Move __cdecl arguments into __usercall registers.
                    emit_load_store(current_register, user_register::ESP, stack_offset);
                    stack_offset -= 4;
                }
                else
                {
                    // Push __usercall registers into __cdecl arguments.
                    emit_push(current_register);
                }
            }
        }

        // Reserve call to hook/original function.
        // Uses a far call to return to the original function so we can mutate
        // the return address (e.g. user uninstalls the hook inside the hook).
        auto branch_offset = emit_branch(true, in_to_original);
        
        if (in_to_original)
        {
            // Clean up __usercall stack arguments.
            if (!in_info.is_userpurge && stack_param_count > 0)
                emit_add_subtract(user_register::ESP, stack_param_count * 4);
            
            // Restore protected registers.
            for (auto i = register_count; i-- > 0;)
            {
                const auto current_register = in_info.get_register(i);

                if (!is_protected_register(current_register))
                    continue;
                
                emit_pop(current_register);
            }
        }

        const auto return_register = in_info.get_return_register();
        
        if (return_register != user_register::none || in_info.is_return_type<void>())
        {
            // Move __cdecl return value into __usercall return register and vice-versa.
            if ((in_info.is_return_type<float>() || in_info.is_return_type<double>()))
            {
                if (return_register != user_register::ST0)
                {
                    if (in_to_original)
                    {
                        emit_move_register(user_register::ST0, return_register, in_info.return_type_size);
                    }
                    else
                    {
                        emit_move_register(return_register, user_register::ST0, in_info.return_type_size);
                    }
                }
            }
            else if (return_register != user_register::EAX)
            {
                if (in_to_original)
                {
                    emit_move_register(user_register::EAX, return_register);
                }
                else
                {
                    emit_move_register(return_register, user_register::EAX);
                }
            }
        }
        
        if (in_to_original)
        {
            emit_return();
        }
        else
        {
            // Pop register arguments.
            for (size_t i = 0; i < param_count; i++)
            {
                if (in_info.is_stack_param(i))
                    continue;

                const auto current_register = in_info.get_param_register(i);

                if (current_register == return_register)
                {
                    emit_add_subtract(user_register::ESP, 4);
                    continue;
                }

                emit_pop(current_register);
            }

            if (stack_param_count > 0)
            {
                stack_offset = stack_param_count * 4;

                // Restore stack pointer for original call.
                emit_add_subtract(user_register::ESP, stack_offset);

                // Pop stack arguments in __userpurge.
                emit_return(in_info.is_userpurge ? stack_offset : 0);
            }
            else
            {
                emit_return();
            }
        }

        const auto size = result.size();

        auto trampoline = _aligned_malloc(size, sizeof(void*));

        memcpy_s(trampoline, size, result.data(), size);

        emit_branch_address(trampoline, branch_offset, in_address, in_to_original);
        
        mem::protect(trampoline, size, mem::get_protect_flags(mem::page_protection::rwx));

        return trampoline;
    }
}
