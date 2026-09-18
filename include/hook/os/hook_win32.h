#pragma once

#include <functional>

#include "../thirdparty/Detours/src/detours.h"
#include "mem/mem.h"
#include "os/win32.h"

#define __CMNLIB_INTERNAL_STATIC_LIB_ENROLMENT

#define __CMNLIB_INTERNAL_STATIC_HOOK_IMPL(NAME, ADDRESS, INSTALLER) \
    static bool result_##NAME{};                                     \
    static bool install_##NAME()                                     \
    {                                                                \
        if (!ADDRESS)                                                \
            return result_##NAME = false;                            \
                                                                     \
        return result_##NAME = INSTALLER(NAME);                      \
    }                                                                \
    static bool runner_##NAME = install_##NAME();

///
/// \class __CMNLIB_INTERNAL_HOOK_COMMON_PARAMS
///
/// \param RETURN_TYPE        The return type of the function.
/// \param CALLING_CONVENTION The calling convention of the function (e.g.
///                           `__cdecl`, `__stdcall`, `__fastcall`, etc).
///                           For custom calling conventions on x86, such as
///                           `__usercall` or `__userpurge`, see
///                           \ref hook_usercall_win32.h.
/// \param FUNCTION_NAME      The name of the function.
/// \param ADDRESS            The address of the function.
/// \param __VA_ARGS__        The parameters of the function.
///

///
/// Declares a pointer to a function in memory.
///
/// \copydoc __CMNLIB_INTERNAL_HOOK_COMMON_PARAMS
///
#define FUNCTION_PTR(RETURN_TYPE, CALLING_CONVENTION, FUNCTION_NAME, ADDRESS, ...) \
    RETURN_TYPE (CALLING_CONVENTION* FUNCTION_NAME)(__VA_ARGS__) = (RETURN_TYPE (CALLING_CONVENTION*)(__VA_ARGS__))(ADDRESS)

///
/// Loads a dynamic link library into memory and declares a pointer to an exported
/// function from it.
///
/// \param RETURN_TYPE        The return type of the function.
/// \param LIBRARY_NAME       The name of the dynamic link library.
/// \param FUNCTION_NAME      The name of the exported function.
/// \param __VA_ARGS__        The parameters of the exported function.
///
#define IMPORT_FUNCTION_PTR(RETURN_TYPE, LIBRARY_NAME, FUNCTION_NAME, ...) \
    typedef RETURN_TYPE _##FUNCTION_NAME(__VA_ARGS__);                     \
    _##FUNCTION_NAME* FUNCTION_NAME = (_##FUNCTION_NAME*)IMPORT_FUNC(LIBRARY_NAME, #FUNCTION_NAME);

///
/// Defines the body of a hook for a function in memory.
///
/// \copydoc __CMNLIB_INTERNAL_HOOK_COMMON_PARAMS
///
#define HOOK(RETURN_TYPE, CALLING_CONVENTION, FUNCTION_NAME, ADDRESS, ...) \
    typedef RETURN_TYPE CALLING_CONVENTION FUNCTION_NAME(__VA_ARGS__);     \
    FUNCTION_NAME* x_##FUNCTION_NAME = (FUNCTION_NAME*)(ADDRESS);          \
    FUNCTION_NAME* original_##FUNCTION_NAME = x_##FUNCTION_NAME;           \
    void* post_##FUNCTION_NAME{};                                          \
    RETURN_TYPE CALLING_CONVENTION impl_##FUNCTION_NAME(__VA_ARGS__)

///
/// Defines the body of a hook for a function in memory, and installs it
/// upon initialisation.
///
/// \copydoc __CMNLIB_INTERNAL_HOOK_COMMON_PARAMS
///
/// \returns Use \ref GET_STATIC_HOOK_RESULT.
///
#define STATIC_HOOK(RETURN_TYPE, CALLING_CONVENTION, FUNCTION_NAME, ADDRESS, ...) \
    HOOK(RETURN_TYPE, CALLING_CONVENTION, FUNCTION_NAME, ADDRESS, __VA_ARGS__);   \
    __CMNLIB_INTERNAL_STATIC_HOOK_IMPL(FUNCTION_NAME, ADDRESS, INSTALL_HOOK)      \
    RETURN_TYPE CALLING_CONVENTION impl_##FUNCTION_NAME(__VA_ARGS__)

///
/// Gets the installation result of a hook defined with \ref STATIC_HOOK,
/// \ref STATIC_USER_HOOK or \ref STATIC_ASM_HOOK.
///
/// \param FUNCTION_NAME The name of the function that was hooked.
///
/// \returns `true` if the installation succeeeded. Otherwise, `false`.
///
#define GET_STATIC_HOOK_RESULT(FUNCTION_NAME) \
    result_##FUNCTION_NAME

///
/// An alias for `__fastcall`.
///
/// This is intended for use with the other `THISCALL` macros to make setting up
/// function pointers and hooks with this calling convention easier.
///
/// `__thiscall` is a calling convention that stores a pointer to `this` in `ECX`,
/// before falling back to using the stack, similar to `__fastcall`.
///
/// > [!NOTE]
/// > **For x64:** This calling convention is always `__fastcall` regardless, with
/// > the class pointer stored in `RCX`.
///
#define THISCALL __fastcall

///
/// \class __CMNLIB_INTERNAL_THISCALL_PARAMS_COMMON
///
/// > [!NOTE]
/// > **For x86:** This macro creates a discard parameter to fill in for `EDX`,
/// > allowing the rest of the arguments to be pushed onto the stack.  
/// > **For x64:** This macro uses standard `__fastcall` conventions.
///

///
/// \def   THISCALL_PARAMS
/// \brief Expands to the necessary function parameters required by `__thiscall`
///        calling convention.
/// \hideinitializer
///
/// \param CLASS_NAME The name of the class this function is from.
///
/// \copydoc __CMNLIB_INTERNAL_THISCALL_PARAMS_COMMON
///
/// ## Examples
/// \code{.cpp}
/// FUNCTION_PTR(int, THISCALL, my_class_function, 0xDEADBEEF, THISCALL_PARAMS(my_class_name), int in_second_param);
/// \endcode
///

///
/// \def   THISCALL_RETURN_PARAMS
/// \brief Expands to the necessary function arguments required by `__thiscall`
///        calling convention for calling the original function.
/// \hideinitializer
///
/// \copydoc __CMNLIB_INTERNAL_THISCALL_PARAMS_COMMON
///
/// ## Examples
/// \code{.cpp}
/// HOOK(int, THISCALL, my_class_function_hook, 0xDEADBEEF, THISCALL_PARAMS(my_class_name), int in_second_param)
/// {
///     return original_my_class_function_hook(THISCALL_RETURN_PARAMS, in_secondParam);
/// }
/// \endcode
///

#if defined(CMNLIB_X64)
#define THISCALL_PARAMS(CLASS_NAME) CLASS_NAME* self
#define THISCALL_RETURN_PARAMS      self
#elif defined(CMNLIB_X86)
#define THISCALL_PARAMS(CLASS_NAME) CLASS_NAME* self, void* _
#define THISCALL_RETURN_PARAMS      self, _
#endif

///
/// \class __CMNLIB_INTERNAL_VFTABLE_HOOK_COMMON_PARAMS
///
/// \param RETURN_TYPE        The return type of the function.
/// \param CALLING_CONVENTION The calling convention of the function (e.g.
///                           `__cdecl`, `__stdcall`, `__fastcall`, etc).
///                           This does not support optimised calling conventions,
///                           such as `__usercall` or `__userpurge`.
/// \param CLASS_NAME         The name of the class that contains the function
///                           being hooked.
/// \param FUNCTION_NAME      The name of the function.
/// \param __VA_ARGS__        The parameters of the function.
///

///
/// Defines the body of a hook for a function in a virtual function table in memory.
///
/// \copydoc __CMNLIB_INTERNAL_VFTABLE_HOOK_COMMON_PARAMS
///
#define VFTABLE_HOOK(RETURN_TYPE, CALLING_CONVENTION, CLASS_NAME, FUNCTION_NAME, ...) \
    HOOK(RETURN_TYPE, CALLING_CONVENTION, CLASS_NAME##_##FUNCTION_NAME, nullptr, __VA_ARGS__)

///
/// Defines the body of a hook for a function in a virtual function table in memory,
/// and installs it upon initialisation.
///
/// \copydoc __CMNLIB_INTERNAL_VFTABLE_HOOK_COMMON_PARAMS
///
/// \returns Use \ref GET_STATIC_VFTABLE_HOOK_RESULT.
///
#define STATIC_VFTABLE_HOOK(RETURN_TYPE, CALLING_CONVENTION, CLASS_NAME, FUNCTION_NAME, ...) \
    STATIC_HOOK(RETURN_TYPE, CALLING_CONVENTION, CLASS_NAME##_##FUNCTION_NAME, nullptr, __VA_ARGS__)

///
/// Gets the installation result of a hook defined with \ref STATIC_VFTABLE_HOOK.
///
/// \param CLASS_NAME    The name of the class that contains the function that
///                      was hooked.
/// \param FUNCTION_NAME The name of the function that was hooked.
///
/// \returns `true` if the installation succeeeded. Otherwise, `false`.
///
#define GET_STATIC_VFTABLE_HOOK_RESULT(CLASS_NAME, FUNCTION_NAME) \
    result_##CLASS_NAME##_##FUNCTION_NAME

///
/// \class __CMNLIB_INTERNAL_ASM_HOOK_COMMON
///
/// > [!NOTE]
/// > **For x64:** The body must be defined in an `*.asm` file using MASM. This
/// > macro creates a scope for members of the hook to be declared in. If no
/// > members are needed, close the scope immediately using empty braces.
///
/// ## Examples
/// ### x86
/// `mod.cpp`
/// \code{.cpp}
/// ASM_HOOK(my_asm_hook, 0xDEADBEEF)
/// {
///     static uint32_t g_my_hook_member = 0x900DF00D;
/// 
///     __asm
///     {
///         mov eax, [g_my_hook_member]
///     }
/// 
///     ASM_HOOK_BRANCH(my_asm_hook)
/// }
/// \endcode
///
/// ### x64
/// `mod.cpp`
/// \code{.cpp}
/// ASM_HOOK(my_asm_hook, 0xDEADBEEF)
/// {
///     uint64_t g_my_hook_member = 0x900DF00D;
/// }
/// \endcode
///
/// `my_asm_hook.asm`
/// \code{.asm}
/// include CommonLib.masm.inc
/// 
/// EXTERN g_my_hook_member : QWORD
/// 
/// ASM_HOOK_START my_asm_hook
///     mov rax, [g_my_hook_member]
///     ASM_HOOK_BRANCH my_asm_hook
/// ASM_HOOK_END my_asm_hook
/// 
/// END
/// \endcode
///

///
/// \class __CMNLIB_INTERNAL_ASM_HOOK_RETURN_BRANCH_COMMON
///
/// > [!NOTE]
/// > **For x64:** This macro can be accessed in an `*.asm` file by
/// > including `CommonLib.masm.inc`.
///

///
/// \def   ASM_HOOK
/// \brief Creates an assembly hook.
/// \hideinitializer
///
/// \param NAME    The name of the hook.
/// \param ADDRESS The address to hook.
///
/// \copydoc __CMNLIB_INTERNAL_ASM_HOOK_COMMON
///

///
/// \def   STATIC_ASM_HOOK
/// \brief Creates an assembly hook, and installs it upon initialisation.
/// \hideinitializer
///
/// \param NAME    The name of the hook.
/// \param ADDRESS The address to hook.
///
/// \copydoc __CMNLIB_INTERNAL_ASM_HOOK_COMMON
///

///
/// \def   ASM_HOOK_RETURN
/// \brief Returns from an assembly hook and executes the original code.
/// \hideinitializer
///
/// \param NAME The name of the hook.
///
/// \copydoc __CMNLIB_INTERNAL_ASM_HOOK_RETURN_BRANCH_COMMON
///

///
/// \def   ASM_HOOK_BRANCH
/// \brief Returns from an assembly hook and skips the original code.
/// \hideinitializer
///
/// \param NAME The name of the hook.
///
/// \copydoc __CMNLIB_INTERNAL_ASM_HOOK_RETURN_BRANCH_COMMON
///

#ifdef CMNLIB_X64

#define ASM_HOOK(NAME, ADDRESS)                          \
    extern "C" uintptr_t x_##NAME = (uint64_t)(ADDRESS); \
    extern "C" uintptr_t original_##NAME = x_##NAME;     \
    extern "C" uintptr_t post_##NAME{};                  \
    extern "C" void* impl_##NAME;                        \
    extern "C"

#define STATIC_ASM_HOOK(NAME, ADDRESS)                               \
    ASM_HOOK(NAME, ADDRESS) {}                                       \
    __CMNLIB_INTERNAL_STATIC_HOOK_IMPL(NAME, ADDRESS, INSTALL_HOOK); \
    extern "C"

#endif // CMNLIB_X64

#ifdef CMNLIB_X86

#define ASM_HOOK(NAME, ADDRESS)        \
    void* x_##NAME = (void*)(ADDRESS); \
    void* original_##NAME = x_##NAME;  \
    void* post_##NAME{};               \
    void NAKED impl_##NAME()

#define STATIC_ASM_HOOK(NAME, ADDRESS)                               \
    void* x_##NAME = (void*)(ADDRESS);                               \
    void* original_##NAME = x_##NAME;                                \
    void* post_##NAME{};                                             \
    void impl_##NAME();                                              \
    __CMNLIB_INTERNAL_STATIC_HOOK_IMPL(NAME, ADDRESS, INSTALL_HOOK); \
    void NAKED impl_##NAME()

#define ASM_HOOK_RETURN(NAME) __asm jmp original_##NAME

#define ASM_HOOK_BRANCH(NAME) __asm jmp post_##NAME

#endif // CMNLIB_X86

///
/// Installs a hook defined with \ref HOOK or \ref ASM_HOOK.
///
/// \param FUNCTION_NAME The name of the function to call before the original.
///
/// \returns `true` if the installation succeeeded, or if the hook was already
///          installed. Otherwise, `false`.
///
#define INSTALL_HOOK(FUNCTION_NAME) \
    INSTALL_HOOK_EXPLICIT(FUNCTION_NAME, original_##FUNCTION_NAME)

///
/// Installs a hook defined with \ref HOOK or \ref ASM_HOOK at an explicit address.
///
/// \param FUNCTION_NAME The name of the function to call before the original.
/// \param ADDRESS       The address of the function to hook.
///
/// \returns `true` if the installation succeeeded, or if the hook was already
///          installed. Otherwise, `false`.
///
#define INSTALL_HOOK_EXPLICIT(FUNCTION_NAME, ADDRESS)                                                       \
    std::invoke([&]() -> bool                                                                               \
    {                                                                                                       \
        if (!original_##FUNCTION_NAME && !(ADDRESS))                                                        \
            return false;                                                                                   \
                                                                                                            \
        *(void**)&original_##FUNCTION_NAME = (void*)(ADDRESS);                                              \
                                                                                                            \
        DetourTransactionBegin();                                                                           \
        DetourUpdateThread(GetCurrentThread());                                                             \
        DetourAttach((void**)&original_##FUNCTION_NAME, &impl_##FUNCTION_NAME);                             \
                                                                                                            \
        const auto result = DetourTransactionCommit() == NO_ERROR;                                          \
                                                                                                            \
        if (result)                                                                                         \
            post_##FUNCTION_NAME = hedgedev::csl::hook::_get_post_hook_address((void*)(x_##FUNCTION_NAME)); \
                                                                                                            \
        return result;                                                                                      \
    })

///
/// Uninstalls a hook installed with \ref INSTALL_HOOK.
///
/// \param FUNCTION_NAME The name of the function to unhook.
///
/// \returns `true` if the uninstallation succeeeded, or if the hook was already
///          uninstalled. Otherwise, `false`.
///
#define UNINSTALL_HOOK(FUNCTION_NAME)                                           \
    std::invoke([&]() -> bool                                                   \
    {                                                                           \
        if (x_##FUNCTION_NAME == original_##FUNCTION_NAME)                      \
            return true;                                                        \
                                                                                \
        DetourTransactionBegin();                                               \
        DetourUpdateThread(GetCurrentThread());                                 \
        DetourDetach((void**)&original_##FUNCTION_NAME, &impl_##FUNCTION_NAME); \
                                                                                \
        return DetourTransactionCommit() == NO_ERROR;                           \
    })

///
/// Installs a hook defined with \ref VFTABLE_HOOK.
///
/// \param CLASS_NAME     The name of the class that contains the function
///                       being hooked.
/// \param INSTANCE       A pointer to an instance of the class to extract the
///                       virtual function table pointer from.
/// \param FUNCTION_NAME  The name of the function to call before the original.
/// \param FUNCTION_INDEX The index of the function to hook.
///
/// \returns `true` if the installation succeeeded, or if the hook was already
///          installed. Otherwise, `false`.
///
#define INSTALL_VFTABLE_HOOK(CLASS_NAME, INSTANCE, FUNCTION_NAME, FUNCTION_INDEX)                               \
    std::invoke([&]() -> bool                                                                                   \
    {                                                                                                           \
        if (original_##CLASS_NAME##_##FUNCTION_NAME)                                                            \
            return true;                                                                                        \
                                                                                                                \
        original_##CLASS_NAME##_##FUNCTION_NAME = (*(CLASS_NAME##_##FUNCTION_NAME***)INSTANCE)[FUNCTION_INDEX]; \
                                                                                                                \
        DetourTransactionBegin();                                                                               \
        DetourUpdateThread(GetCurrentThread());                                                                 \
        DetourAttach((void**)&original_##CLASS_NAME##_##FUNCTION_NAME, impl_##CLASS_NAME##_##FUNCTION_NAME);    \
                                                                                                                \
        const auto result = DetourTransactionCommit() == NO_ERROR;                                              \
                                                                                                                \
        if (result)                                                                                             \
            post_##FUNCTION_NAME = hedgedev::csl::hook::_get_post_hook_address((void*)(x_##FUNCTION_NAME));     \
                                                                                                                \
        return result;                                                                                          \
    })

///
/// Uninstalls a hook installed with \ref INSTALL_VFTABLE_HOOK.
///
/// \param CLASS_NAME    The name of the class that contains the function that
///                      was hooked.
/// \param FUNCTION_NAME The name of the function to unhook.
///
/// \returns `true` if the uninstallation succeeeded, or if the hook was already
///          uninstalled. Otherwise, `false`.
///
#define UNINSTALL_VFTABLE_HOOK(CLASS_NAME, FUNCTION_NAME) \
    UNINSTALL_HOOK(CLASS_NAME##_##FUNCTION_NAME)

namespace hedgedev::csl::hook
{
    inline void* _get_post_hook_address(void* in_hook)
    {
        uint8_t* result{};

        if (!in_hook)
            return result;
        
        const auto branch_info = mem::get_branch_info(in_hook);

        result = (uint8_t*)(uintptr_t(in_hook) + branch_info.instr_length);

        while (*result == 0xCC)
            result++;

        return result;
    }
}

#ifdef CMNLIB_X86
#include "hook_usercall_win32.h"
#endif
