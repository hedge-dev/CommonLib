///
/// \file HookingUserCall.h
/// 
/// Macros for creating hooks for optimised functions on x86.
///

#pragma once

#include <array>
#include <bit>
#include <optional>
#include <typeindex>
#include <vector>

///
/// A custom calling convention that stores arguments in optimised locations
/// before falling back to using the stack, similar to `__fastcall`.
///
/// This calling convention puts the responsibility of cleaning up stack
/// arguments on the caller.
///
/// This identifier cannot be used as a standard calling convention.
///
#define __usercall 0

///
/// A custom calling convention that stores arguments in optimised locations
/// before falling back to using the stack, similar to `__fastcall`.
///
/// This calling convention puts the responsibility of cleaning up stack
/// arguments on the callee.
///
/// This identifier cannot be used as a standard calling convention.
///
#define __userpurge 1

#define USER_REGISTER(REGISTER) \
    ((uint64_t)(hedgedev::csl::hook::UserRegister::REGISTER))

///
/// Specifies the hook has no return value.
///
#define USER_RETURN_VOID \
    USER_REGISTER(None)

///
/// Specify the register to use for the return value.
/// 
/// \param REGISTER The register to use for the return value.
///
#define USER_RETURN(REGISTER) \
    USER_REGISTER(REGISTER)

///
/// Specify the register to use for a specific parameter.
/// 
/// \param INDEX    The index of the parameter.
/// \param REGISTER The register to use for the parameter.
///
#define USER_PARAM(INDEX, REGISTER) \
    (USER_REGISTER(REGISTER) << ((INDEX + 1) * hedgedev::csl::hook::g_kUserRegisterSize))

///
/// Declares a pointer to a function with custom calling convention in memory.
/// 
/// \param RETURN_TYPE        The return type of the function.
/// \param CALLING_CONVENTION The calling convention of the function, such as `__usercall` or `__userpurge`.
/// \param FUNCTION_NAME      The name of the function.
/// \param ADDRESS            The address of the function.
/// \param REGISTERS          The registers used by the return value and parameters.
/// \param PARAM_COUNT        The total number of parameters in \ref __VA_ARGS__.
/// \param __VA_ARGS__        The parameters of the function.
///
#define USER_FUNCTION_PTR(RETURN_TYPE, CALLING_CONVENTION, FUNCTION_NAME, ADDRESS, REGISTERS, PARAM_COUNT, ...)                                                                          \
    hedgedev::csl::hook::UserCallInfo info_##FUNCTION_NAME { typeid(RETURN_TYPE), sizeof(RETURN_TYPE), CALLING_CONVENTION, (void*)(ADDRESS), (void*)(ADDRESS), REGISTERS, PARAM_COUNT }; \
    void* trampoline_##FUNCTION_NAME = hedgedev::csl::hook::EmitUserTrampoline(info_##FUNCTION_NAME, size_t(&info_##FUNCTION_NAME.fpOriginal), true);                                    \
    FUNCTION_PTR(RETURN_TYPE, __cdecl, FUNCTION_NAME, trampoline_##FUNCTION_NAME, __VA_ARGS__)

///
/// Defines the body of a hook for a function with custom calling convention in memory.
/// 
/// \param RETURN_TYPE        The return type of the function.
/// \param CALLING_CONVENTION The calling convention of the function, such as `__usercall` or `__userpurge`.
/// \param FUNCTION_NAME      The name of the function.
/// \param ADDRESS            The address of the function.
/// \param REGISTERS          The registers used by the return value and parameters.
/// \param PARAM_COUNT        The total number of parameters in \ref __VA_ARGS__.
/// \param __VA_ARGS__        The parameters of the function.
///
#define USER_HOOK(RETURN_TYPE, CALLING_CONVENTION, FUNCTION_NAME, ADDRESS, REGISTERS, PARAM_COUNT, ...)                                                                                  \
    hedgedev::csl::hook::UserCallInfo info_##FUNCTION_NAME { typeid(RETURN_TYPE), sizeof(RETURN_TYPE), CALLING_CONVENTION, (void*)(ADDRESS), (void*)(ADDRESS), REGISTERS, PARAM_COUNT }; \
    RETURN_TYPE __cdecl impl_##FUNCTION_NAME(__VA_ARGS__);                                                                                                                               \
    void* trampolineToHook_##FUNCTION_NAME = hedgedev::csl::hook::EmitUserTrampoline(info_##FUNCTION_NAME, size_t(&impl_##FUNCTION_NAME));                                               \
    HOOK(RETURN_TYPE, __cdecl, FUNCTION_NAME, nullptr, __VA_ARGS__)

///
/// Defines the body of a hook for a function with custom calling convention in memory, and installs it upon initialisation.
/// 
/// \param RETURN_TYPE        The return type of the function.
/// \param CALLING_CONVENTION The calling convention of the function, such as `__usercall` or `__userpurge`.
/// \param FUNCTION_NAME      The name of the function.
/// \param ADDRESS            The address of the function.
/// \param REGISTERS          The registers used by the return value and parameters.
/// \param PARAM_COUNT        The total number of parameters in \ref __VA_ARGS__.
/// \param __VA_ARGS__        The parameters of the function.
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
/// \returns `true` if the installation succeeeded, or if the hook was already installed. Otherwise, `false`.
///
#define INSTALL_USER_HOOK(FUNCTION_NAME) \
    INSTALL_USER_HOOK_EXPLICIT(FUNCTION_NAME, info_##FUNCTION_NAME.fpDetour)

///
/// Installs a hook defined with \ref USER_HOOK at an explicit address.
/// 
/// \param FUNCTION_NAME The name of the function to call before the original.
/// \param ADDRESS       The address of the function to hook.
/// 
/// \returns `true` if the installation succeeeded, or if the hook was already installed. Otherwise, `false`.
///
#define INSTALL_USER_HOOK_EXPLICIT(FUNCTION_NAME, ADDRESS)                                                                  \
    std::invoke([&]()                                                                                                       \
    {                                                                                                                       \
        const auto& rInfo = info_##FUNCTION_NAME;                                                                           \
                                                                                                                            \
        if (!rInfo.fpDetour && !(ADDRESS))                                                                                  \
            return false;                                                                                                   \
                                                                                                                            \
        *(void**)&rInfo.fpDetour = (void*)(ADDRESS);                                                                        \
                                                                                                                            \
        DetourTransactionBegin();                                                                                           \
        DetourUpdateThread(GetCurrentThread());                                                                             \
        DetourAttach((void**)&rInfo.fpDetour, trampolineToHook_##FUNCTION_NAME);                                            \
                                                                                                                            \
        const auto result = DetourTransactionCommit() == NO_ERROR;                                                          \
                                                                                                                            \
        *(void**)&original_##FUNCTION_NAME = hedgedev::csl::hook::EmitUserTrampoline(rInfo, size_t(&rInfo.fpDetour), true); \
                                                                                                                            \
        return result;                                                                                                      \
    })

///
/// Uninstalls a hook installed with \ref INSTALL_USER_HOOK.
/// 
/// \param FUNCTION_NAME The name of the function to unhook.
/// 
/// \returns `true` if the uninstallation succeeeded, or if the hook was already uninstalled. Otherwise, `false`.
///
#define UNINSTALL_USER_HOOK(FUNCTION_NAME)                                       \
    std::invoke([&]()                                                            \
    {                                                                            \
        const auto& rInfo = info_##FUNCTION_NAME;                                \
                                                                                 \
        if (rInfo.fpOriginal == rInfo.fpDetour)                                  \
            return true;                                                         \
                                                                                 \
        DetourTransactionBegin();                                                \
        DetourUpdateThread(GetCurrentThread());                                  \
        DetourDetach((void**)&rInfo.fpDetour, trampolineToHook_##FUNCTION_NAME); \
                                                                                 \
        return DetourTransactionCommit() == NO_ERROR;                            \
    })

namespace hedgedev::csl::hook
{
    ///
    /// Registers used for `__usercall`/`__userpurge` calling conventions.
    /// 
    /// These registers have been ordered in an specific way that line up with
    /// register IDs used for encoding instructions.
    ///
    enum class UserRegister : uint64_t
    {
        None,

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

        Count
    };

    enum class UserRegisterFamily : uint64_t
    {
        None,
        GPR = UserRegister::EAX,
        FPU = UserRegister::ST0,
        SSE = UserRegister::XMM0
    };

    ///
    /// The total number of bits in the register flags.
    ///
    inline static constexpr size_t g_kUserRegisterBitsLength = sizeof(uint64_t) * sizeof(uint64_t);

    ///
    /// The total number of bits per register in the register flags.
    ///
    inline static constexpr size_t g_kUserRegisterSize = std::bit_width(uint64_t(UserRegister::Count));

    ///
    /// The maximum number of registers that can fit in the register flags.
    ///
    inline static constexpr size_t g_kUserRegisterMax = g_kUserRegisterBitsLength / g_kUserRegisterSize;

    ///
    /// The number of remaining bits when all slots are used up in the register flags.
    ///
    inline static constexpr size_t g_kUserRegisterBitsRemainder = g_kUserRegisterBitsLength % g_kUserRegisterSize;

    ///
    /// The mask for getting the register bits.
    ///
    inline static constexpr uint64_t g_kUserRegisterMask = 0x1F;

    struct UserCallInfo
    {
        std::type_index ReturnType;
        size_t ReturnTypeSize{};
        bool IsUserPurge{};
        void* fpOriginal{};
        void* fpDetour{};
        uint64_t Registers{};
        uint8_t ParamCount{};

        size_t GetRegisterCount() const
        {
            return size_t(std::bit_width(Registers) + g_kUserRegisterBitsRemainder) / g_kUserRegisterSize;
        }

        size_t GetRegisterParamCount() const
        {
            const auto registerCount = GetRegisterCount();

            if (registerCount <= 1)
                return 0;

            return registerCount - 1;
        }

        size_t GetStackParamCount() const
        {
            return ParamCount - GetRegisterParamCount();
        }

        size_t GetParamCount() const
        {
            return GetRegisterParamCount() + GetStackParamCount();
        }

        UserRegister GetRegister(int in_index) const
        {
            return UserRegister(Registers >> (g_kUserRegisterSize * in_index) & g_kUserRegisterMask);
        }

        UserRegister GetReturnRegister() const
        {
            return GetRegister(0);
        }
        
        UserRegister GetParamRegister(int in_index) const
        {
            return GetRegister(in_index + 1);
        }

        bool IsProtectedRegister(UserRegister in_register) const
        {
            return in_register == UserRegister::EBX ||
                   in_register == UserRegister::EBP ||
                   in_register == UserRegister::ESI ||
                   in_register == UserRegister::EDI;
        }

        template <typename T>
        bool IsReturnType() const
        {
            return typeid(T) == ReturnType;
        }

        bool IsStackParam(int in_index) const
        {
            return GetRegisterParamCount() < size_t(in_index + 1);
        }
    };

    ///
    /// Gets the family a register belongs to.
    /// 
    /// \param in_register The register to check.
    ///
    inline UserRegisterFamily GetRegisterFamily(UserRegister in_register)
    {
        if (in_register >= UserRegister::EAX && in_register <= UserRegister::EDI)
        {
            return UserRegisterFamily::GPR;
        }
        else if (in_register >= UserRegister::ST0 && in_register <= UserRegister::ST7)
        {
            return UserRegisterFamily::FPU;
        }
        else if (in_register >= UserRegister::XMM0 && in_register <= UserRegister::XMM7)
        {
            return UserRegisterFamily::SSE;
        }

        return UserRegisterFamily::None;
    }

    ///
    /// Gets the ID of a register.
    /// 
    /// \param in_register The register to get the ID for.
    ///
    inline int GetRegisterID(UserRegister in_register)
    {
        return int(in_register) - int(GetRegisterFamily(in_register));
    }
    
    ///
    /// Emits a trampoline that forwards arguments from a function with custom calling convention to a
    /// `__cdecl` function in a hook defined with \ref USER_HOOK.
    /// 
    /// \param in_rInfo        The info about the hook.
    /// \param in_address      The address of the `__cdecl` function to trampoline to.
    /// \param in_isToOriginal Determines whether this trampoline is going back to the original function.
    /// 
    /// \returns A pointer to the trampoline that was emitted.
    ///
    inline void* EmitUserTrampoline(const UserCallInfo& in_rInfo, const size_t in_address, bool in_isToOriginal = false)
    {
        std::vector<uint8_t> result{};

        const auto emitImm8 = [&](int in_value)
        {
            auto pos = result.size();

            result.push_back(in_value & 0xFF);

            return pos;
        };
        
        const auto emitImm16 = [&](int in_value)
        {
            auto pos = emitImm8(in_value);

            result.push_back((in_value >> 8) & 0xFF);

            return pos;
        };

        const auto emitImm32 = [&](int in_value)
        {
            auto pos = emitImm16(in_value);

            result.push_back((in_value >> 16) & 0xFF);
            result.push_back((in_value >> 24) & 0xFF);

            return pos;
        };

        const auto emitPush = [&](UserRegister in_register, std::optional<int> in_offset = {})
        {
            const auto registerId = GetRegisterID(in_register);

            if (in_offset.has_value())
            {
                const auto offset = in_offset.value();
                const auto emitEbpNoOffset = in_register == UserRegister::EBP && offset == 0;

                result.push_back(0xFF);

                if (!offset && !emitEbpNoOffset)
                {
                    // push [{register}]
                    result.push_back(uint8_t(0x30 | registerId));
                }
                else if (offset <= 0x7F || emitEbpNoOffset)
                {
                    // push [{register} + {imm8}]
                    result.push_back(uint8_t(0x70 | registerId));
                }
                else
                {
                    // push [{register} + {imm32}]
                    result.push_back(uint8_t(0xB0 | registerId));
                }

                if (in_register == UserRegister::ESP)
                    result.push_back(0x24);

                if (!offset && !emitEbpNoOffset)
                    return;
                
                if ((offset > 0 && offset <= 0x7F) || emitEbpNoOffset)
                {
                    emitImm8(offset);
                }
                else if (offset > 0x7F)
                {
                    emitImm32(offset);
                }

                return;
            }

            // push {register}
            result.push_back(uint8_t(0x50 | registerId));
        };

        const auto emitPop = [&](UserRegister in_register)
        {
            // pop {register}
            result.push_back(uint8_t(0x58 | GetRegisterID(in_register)));
        };

        const auto emitAddSubtract = [&](UserRegister in_register, int in_value)
        {
            if (!in_value)
                return;
            
            const auto registerId = GetRegisterID(in_register);
            const auto isAdd = in_value > 0;
            const auto valueAbs = std::abs(in_value);

            if (valueAbs == 1)
            {
                auto opcode = 0x40;

                if (!isAdd)
                    opcode |= 8;

                // inc/dec {register}
                result.push_back(uint8_t(opcode | registerId));
            }
            else
            {
                auto modifier = 0;

                if (!isAdd)
                    modifier = 0x28;

                if (valueAbs > 0 && valueAbs <= 0x7F)
                {
                    // add/sub {register}, {imm8}
                    result.push_back(0x83);
                    result.push_back(uint8_t(0xC0 | modifier | registerId));
                    emitImm8(valueAbs);
                }
                else if (valueAbs > 0x7F)
                {
                    // add/sub {register}, {imm32}
                    if (in_register == UserRegister::EAX)
                    {
                        result.push_back(uint8_t(0x05 | modifier));
                    }
                    else
                    {
                        result.push_back(0x81);
                        result.push_back(uint8_t(0xC0 | modifier | registerId));
                    }

                    emitImm32(valueAbs);
                }
            }
        };

        const auto emitLoadStoreSSE = [&](UserRegister in_dst, UserRegister in_src, int in_offset = 0, int in_floatSize = sizeof(float), bool in_isStore = false)
        {
            const auto dstRegisterId = GetRegisterID(in_dst);
            const auto srcRegisterId = GetRegisterID(in_src);

            const auto ptrRegister = in_isStore ? in_dst : in_src;
            const auto ptrRegisterId = in_isStore ? dstRegisterId : srcRegisterId;

            const auto dataRegister = in_isStore ? in_src : in_dst;
            const auto dataRegisterId = in_isStore ? srcRegisterId : dstRegisterId;

            const auto emitEbpNoOffset = ptrRegister == UserRegister::EBP && in_offset == 0;

            // movss dword ptr
            auto opcode = 0xF3;

            // movsd qword ptr
            if (in_floatSize == sizeof(double))
                opcode = 0xF2;

            result.push_back(opcode);
            result.push_back(0x0F);
            result.push_back(in_isStore ? 0x11 : 0x10);

            auto modifier = 0;

            if ((in_offset > 0 && in_offset <= 0x7F) || emitEbpNoOffset)
            {
                modifier = 0x40;
            }
            else if (in_offset > 0x7F)
            {
                modifier = 0x80;
            }
            
            if (ptrRegister == UserRegister::ESP)
            {
                result.push_back(uint8_t(0x04 | modifier | (dataRegisterId << 3)));
                result.push_back(0x24);
            }
            else
            {
                result.push_back(uint8_t(modifier | (ptrRegisterId << 3) | dataRegisterId));
            }

            if ((in_offset > 0 && in_offset <= 0x7F) || emitEbpNoOffset)
            {
                // movss/movsd {dst}, dword/qword ptr [{src} + {imm8}]
                // movss/movsd dword/qword ptr [{dst} + {imm8}], {src}
                emitImm8(in_offset);
            }
            else if (in_offset > 0x7F)
            {
                // movss/movsd {dst}, dword/qword ptr [{src} + {imm32}]
                // movss/movsd dword/qword ptr [{dst} + {imm32}], {src}
                emitImm32(in_offset);
            }
        };

        const auto emitExchangeRegisterFPU = [&](UserRegister in_register)
        {
            if (GetRegisterFamily(in_register) != UserRegisterFamily::FPU)
                return;
            
            // fxch {register}
            result.push_back(0xD9);
            result.push_back(uint8_t(0xC8 | GetRegisterID(in_register)));
        };

        const auto emitLoadStoreFPU = [&](UserRegister in_register, int in_offset = 0, int in_floatSize = sizeof(float), bool in_isStore = false)
        {
            const auto registerId = GetRegisterID(in_register);

            switch (GetRegisterFamily(in_register))
            {
                case UserRegisterFamily::GPR:
                {
                    const auto emitEbpNoOffset = in_register == UserRegister::EBP && in_offset == 0;

                    // fld dword ptr
                    auto opcode = 0xD9;

                    // fld qword ptr
                    if (in_floatSize == sizeof(double))
                        opcode |= 4;
                    
                    result.push_back(opcode);
                    
                    auto modifier = 0;

                    if ((in_offset > 0 && in_offset <= 0x7F) || emitEbpNoOffset)
                    {
                        modifier = 0x40;
                    }
                    else if (in_offset > 0x7F)
                    {
                        modifier = 0x80;
                    }

                    // fstp
                    if (in_isStore)
                        modifier |= 0x18;
                    
                    if (in_register == UserRegister::ESP)
                        modifier |= 4;

                    result.push_back(uint8_t(modifier | registerId));
                    
                    if (in_register == UserRegister::ESP)
                        result.push_back(0x24);

                    if ((in_offset > 0 && in_offset <= 0x7F) || emitEbpNoOffset)
                    {
                        // fld/fstp dword/qword ptr [{register} + {imm8}]
                        emitImm8(in_offset);
                    }
                    else if (in_offset > 0x7F)
                    {
                        // fld/fstp dword/qword ptr [{register} + {imm32}]
                        emitImm32(in_offset);
                    }

                    break;
                }

                // fstp {dst}
                case UserRegisterFamily::FPU:
                    result.push_back(0xDD);
                    result.push_back(uint8_t(0xD8 | registerId));
                    break;
            }
        };

        const auto emitLoadStore = [&](UserRegister in_dst, UserRegister in_src, int in_offset = 0, bool in_isStore = false)
        {
            const auto dstRegisterId = GetRegisterID(in_dst);
            const auto srcRegisterId = GetRegisterID(in_src);

            const auto ptrRegister = in_isStore ? in_dst : in_src;
            const auto ptrRegisterId = in_isStore ? dstRegisterId : srcRegisterId;

            const auto dataRegister = in_isStore ? in_src : in_dst;
            const auto dataRegisterId = in_isStore ? srcRegisterId : dstRegisterId;

            const auto emitEbpNoOffset = ptrRegister == UserRegister::EBP && in_offset == 0;

            // mov [{dst}], {src}
            auto opcode = 0x89;

            // mov {dst}, [{src}]
            if (!in_isStore)
                opcode |= 2;

            result.push_back(opcode);
                    
            auto modifier = 0;

            if ((in_offset > 0 && in_offset <= 0x7F) || emitEbpNoOffset)
            {
                modifier = 0x40;
            }
            else if (in_offset > 0x7F)
            {
                modifier = 0x80;
            }
            
            if (ptrRegister == UserRegister::ESP)
            {
                result.push_back(uint8_t(0x04 | modifier | (dataRegisterId << 3)));
                result.push_back(0x24);
            }
            else
            {
                result.push_back(uint8_t(modifier | (ptrRegisterId << 3) | dataRegisterId));
            }

            if ((in_offset > 0 && in_offset <= 0x7F) || emitEbpNoOffset)
            {
                // mov {dst}, [{src} + {imm8}]
                // mov [{dst} + {imm8}], {src}
                emitImm8(in_offset);
            }
            else if (in_offset > 0x7F)
            {
                // mov {dst}, [{src} + {imm32}]
                // mov [{dst} + {imm32}], {src}
                emitImm32(in_offset);
            }
        };

        const auto emitMoveRegisterFPU = [&](UserRegister in_dst, UserRegister in_src, int in_floatSize = sizeof(float))
        {
            const auto dstRegisterId = GetRegisterID(in_dst);
            const auto srcRegisterId = GetRegisterID(in_src);
            const auto dstRegisterFamily = GetRegisterFamily(in_dst);
            const auto srcRegisterFamily = GetRegisterFamily(in_src);

            if (dstRegisterFamily == UserRegisterFamily::FPU)
            {
                if (dstRegisterFamily == srcRegisterFamily)
                {
                    if (in_dst == in_src)
                        return;
                    
                    // Exchange ST(0) with source register.
                    emitExchangeRegisterFPU(in_src);

                    // Copy ST(0) to destination register.
                    emitLoadStoreFPU(in_dst);

                    // Restore ST(0).
                    emitExchangeRegisterFPU(in_src);
                }
                else
                {
                    // Allocate stack space for register.
                    emitAddSubtract(UserRegister::ESP, -in_floatSize);

                    if (srcRegisterFamily == UserRegisterFamily::GPR)
                    {
                        // Copy GPR register into stack.
                        emitLoadStore(UserRegister::ESP, in_src, 0, true);
                    }
                    else if (srcRegisterFamily == UserRegisterFamily::SSE)
                    {
                        // Copy SSE register into stack.
                        emitLoadStoreSSE(UserRegister::ESP, in_src, 0, in_floatSize, true);
                    }
                    
                    // Exchange ST(0) with destination register.
                    if (in_dst != UserRegister::ST0)
                        emitExchangeRegisterFPU(in_dst);

                    // Load stack value into FPU register.
                    emitLoadStoreFPU(UserRegister::ESP, 0, in_floatSize);

                    // Restore ST(0).
                    if (in_dst != UserRegister::ST0)
                        emitExchangeRegisterFPU(in_dst);

                    // Deallocate stack space.
                    emitAddSubtract(UserRegister::ESP, in_floatSize);
                }
            }
            else
            {
                // Allocate stack space for FPU register.
                emitAddSubtract(UserRegister::ESP, -in_floatSize);

                // Exchange ST(0) with source register.
                if (in_src != UserRegister::ST0)
                    emitExchangeRegisterFPU(in_src);

                // Copy ST(0) into stack.
                emitLoadStoreFPU(UserRegister::ESP, 0, in_floatSize, true);

                // Restore ST(0).
                if (in_src != UserRegister::ST0)
                    emitExchangeRegisterFPU(in_src);

                if (dstRegisterFamily == UserRegisterFamily::GPR)
                {
                    // Load stack value into GPR register.
                    emitLoadStore(in_dst, UserRegister::ESP, 0);
                }
                else if (dstRegisterFamily == UserRegisterFamily::SSE)
                {
                    // Load stack value into SSE register.
                    emitLoadStoreSSE(in_dst, UserRegister::ESP, 0, in_floatSize);
                }

                // Deallocate stack space.
                emitAddSubtract(UserRegister::ESP, in_floatSize);
            }
        };

        const auto emitMoveRegisterSSE = [&](UserRegister in_dst, UserRegister in_src, int in_floatSize = sizeof(float))
        {
            const auto dstRegisterId = GetRegisterID(in_dst);
            const auto srcRegisterId = GetRegisterID(in_src);
            const auto dstRegisterFamily = GetRegisterFamily(in_dst);
            const auto srcRegisterFamily = GetRegisterFamily(in_src);

            if (dstRegisterFamily == srcRegisterFamily && dstRegisterFamily == UserRegisterFamily::SSE)
            {
                // movss {dst}, {src}
                auto opcode = 0xF3;

                // movsd {dst}, {src}
                if (in_floatSize == sizeof(double))
                    opcode = 0xF2;
                
                result.push_back(opcode);
                result.push_back(0x0F);
                result.push_back(0x10);
                result.push_back(uint8_t(0xC0 | (srcRegisterId << 3) | dstRegisterId));
            }
            else if ((dstRegisterFamily == UserRegisterFamily::GPR || dstRegisterFamily == UserRegisterFamily::SSE) &&
                     (srcRegisterFamily == UserRegisterFamily::GPR || srcRegisterFamily == UserRegisterFamily::SSE))
            {
                auto modifier = 0x6E;

                if (dstRegisterFamily == UserRegisterFamily::GPR)
                    modifier |= 0x10;

                // movd {dst}, {src}
                result.push_back(0x66);
                result.push_back(0x0F);
                result.push_back(modifier);
                result.push_back(uint8_t(0xC0 | (srcRegisterId << 3) | dstRegisterId));
            }
            else
            {
                emitMoveRegisterFPU(in_dst, in_src, in_floatSize);
            }
        };

        const auto emitMoveRegister = [&](UserRegister in_dst, UserRegister in_src, int in_floatSize = sizeof(float))
        {
            const auto dstRegisterFamily = GetRegisterFamily(in_dst);
            const auto srcRegisterFamily = GetRegisterFamily(in_src);

            if (dstRegisterFamily == srcRegisterFamily)
            {
                switch (dstRegisterFamily)
                {
                    // mov {dst}, {src}
                    case UserRegisterFamily::GPR:
                        result.push_back(0x89);
                        result.push_back(uint8_t(0xC0 | (GetRegisterID(in_src) << 3) | GetRegisterID(in_dst)));
                        break;

                    case UserRegisterFamily::FPU:
                        emitMoveRegisterFPU(in_dst, in_src, in_floatSize);
                        break;

                    case UserRegisterFamily::SSE:
                        emitMoveRegisterSSE(in_dst, in_src, in_floatSize);
                        break;
                }
            }
            else
            {
                emitMoveRegisterSSE(in_dst, in_src, in_floatSize);
            }
        };

        const auto emitBranch = [&](bool in_isCall, bool in_isFar)
        {
            if (in_isFar)
            {
                // jmp/call [{imm32}]
                result.push_back(0xFF);
                result.push_back(in_isCall ? 0x15 : 0x25);
            }
            else
            {
                // jmp/call {imm32}
                result.push_back(in_isCall ? 0xE8 : 0xE9);
            }

            return emitImm32(0);
        };

        const auto emitBranchAddr = [&](void* in_pStart, size_t in_branchOffset, size_t in_target, bool in_isAbsolute)
        {
            const auto offset = size_t(in_pStart) + in_branchOffset;

            if (in_isAbsolute)
            {
                *(uint32_t*)offset = uint32_t(in_target);
            }
            else
            {
                *(uint32_t*)offset = uint32_t((in_target - offset) - sizeof(uint32_t));
            }
        };

        const auto emitReturn = [&](size_t in_size = 0)
        {
            auto opcode = 0xC2;

            if (!in_size)
                opcode |= 1;

            // ret
            result.push_back(opcode);

            if (!in_size)
                return;
            
            // ret {imm16}
            emitImm16(in_size);
        };

        const auto registerCount = in_rInfo.GetRegisterCount();
        const auto stackParamCount = in_rInfo.GetStackParamCount();
        const auto paramCount = in_rInfo.GetParamCount();
        
        auto stackOffset = in_isToOriginal
            ? paramCount * 4
            : stackParamCount * 4;

        if (in_isToOriginal)
        {
            // Back up protected registers.
            for (size_t i = 0; i < registerCount; i++)
            {
                const auto currentRegister = in_rInfo.GetRegister(i);

                if (!in_rInfo.IsProtectedRegister(currentRegister))
                    continue;
                
                emitPush(currentRegister);
                stackOffset += 4;
            }
        }

        // Set up arguments.
        for (auto i = paramCount; i-- > 0;)
        {
            if (in_rInfo.IsStackParam(i))
            {
                emitPush(UserRegister::ESP, stackOffset);
            }
            else
            {
                const auto currentRegister = in_rInfo.GetParamRegister(i);

                if (in_isToOriginal)
                {
                    // Move __cdecl arguments into __usercall registers.
                    emitLoadStore(currentRegister, UserRegister::ESP, stackOffset);
                    stackOffset -= 4;
                }
                else
                {
                    // Push __usercall registers into __cdecl arguments.
                    emitPush(currentRegister);
                }
            }
        }

        // Reserve call to hook/original function.
        // Uses a far call to return to the original function so we can mutate
        // the return address (e.g. user uninstalls the hook inside the hook).
        auto branchOffset = emitBranch(true, in_isToOriginal);
        
        if (in_isToOriginal)
        {
            // Clean up __usercall stack arguments.
            if (!in_rInfo.IsUserPurge && stackParamCount > 0)
                emitAddSubtract(UserRegister::ESP, stackParamCount * 4);
            
            // Restore protected registers.
            for (auto i = registerCount; i-- > 0;)
            {
                const auto currentRegister = in_rInfo.GetRegister(i);

                if (!in_rInfo.IsProtectedRegister(currentRegister))
                    continue;
                
                emitPop(currentRegister);
            }
        }

        const auto returnRegister = in_rInfo.GetReturnRegister();
        
        if (returnRegister != UserRegister::None)
        {
            // Move __cdecl return value into __usercall return register and vice-versa.
            if ((in_rInfo.IsReturnType<float>() || in_rInfo.IsReturnType<double>()))
            {
                if (returnRegister != UserRegister::ST0)
                {
                    if (in_isToOriginal)
                    {
                        emitMoveRegister(UserRegister::ST0, returnRegister, in_rInfo.ReturnTypeSize);
                    }
                    else
                    {
                        emitMoveRegister(returnRegister, UserRegister::ST0, in_rInfo.ReturnTypeSize);
                    }
                }
            }
            else if (returnRegister != UserRegister::EAX)
            {
                if (in_isToOriginal)
                {
                    emitMoveRegister(UserRegister::EAX, returnRegister);
                }
                else
                {
                    emitMoveRegister(returnRegister, UserRegister::EAX);
                }
            }
        }
        
        if (in_isToOriginal)
        {
            emitReturn();
        }
        else
        {
            // Pop register arguments.
            for (size_t i = 0; i < paramCount; i++)
            {
                if (in_rInfo.IsStackParam(i))
                    continue;

                const auto currentRegister = in_rInfo.GetParamRegister(i);

                if (currentRegister == returnRegister)
                {
                    emitAddSubtract(UserRegister::ESP, 4);
                    continue;
                }

                emitPop(in_rInfo.GetParamRegister(i));
            }

            if (stackParamCount > 0)
            {
                stackOffset = stackParamCount * 4;

                // Restore stack pointer for original call.
                emitAddSubtract(UserRegister::ESP, stackOffset);

                // Pop stack arguments in __userpurge.
                emitReturn(in_rInfo.IsUserPurge ? stackOffset : 0);
            }
            else
            {
                emitReturn();
            }
        }

        const auto size = result.size();

        auto pTrampoline = _aligned_malloc(size, sizeof(void*));
        memcpy_s(pTrampoline, size, result.data(), size);

        emitBranchAddr(pTrampoline, branchOffset, in_address, in_isToOriginal);

        DWORD oldProtect{};
        VirtualProtect(pTrampoline, size, PAGE_EXECUTE_READWRITE, &oldProtect);

        return pTrampoline;
    }
}
