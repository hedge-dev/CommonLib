///
/// \file HookingWin32.h
/// 
/// Macros for creating hooks for Microsoft Windows on x86 and x64.
///

#pragma once

#include <functional>
#include <../thirdparty/Detours/src/detours.h>

#define __CMNLIB_INTERNAL_STATIC_LIB_ENROLMENT "Hooking macros on Windows use Detours."

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
/// Declares a pointer to a function in memory.
/// 
/// \param RETURN_TYPE        The return type of the function.
/// \param CALLING_CONVENTION The calling convention of the function (e.g. `__cdecl`, `__stdcall`, `__fastcall`, etc).
///                           This does not support optimised calling conventions, such as `__usercall` or `__userpurge`.
/// \param FUNCTION_NAME      The name of the function.
/// \param ADDRESS            The address of the function.
/// \param __VA_ARGS__        The parameters of the function.
///
#define FUNCTION_PTR(RETURN_TYPE, CALLING_CONVENTION, FUNCTION_NAME, ADDRESS, ...) \
    RETURN_TYPE (CALLING_CONVENTION* FUNCTION_NAME)(__VA_ARGS__) = (RETURN_TYPE (CALLING_CONVENTION*)(__VA_ARGS__))(ADDRESS)

///
/// Loads a dynamic link library into memory and declares a pointer to an exported function from it.
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
/// \param RETURN_TYPE        The return type of the function.
/// \param CALLING_CONVENTION The calling convention of the function (e.g. `__cdecl`, `__stdcall`, `__fastcall`, etc).
///                           This does not support optimised calling conventions, such as `__usercall` or `__userpurge`.
/// \param FUNCTION_NAME      The name of the function.
/// \param ADDRESS            The address of the function.
/// \param __VA_ARGS__        The parameters of the function.
///
#define HOOK(RETURN_TYPE, CALLING_CONVENTION, FUNCTION_NAME, ADDRESS, ...) \
    typedef RETURN_TYPE CALLING_CONVENTION FUNCTION_NAME(__VA_ARGS__);     \
    FUNCTION_NAME* x_##FUNCTION_NAME = (FUNCTION_NAME*)(ADDRESS);          \
    FUNCTION_NAME* original_##FUNCTION_NAME = x_##FUNCTION_NAME;           \
    RETURN_TYPE CALLING_CONVENTION impl_##FUNCTION_NAME(__VA_ARGS__)

///
/// Defines the body of a hook for a function in memory, and installs it upon initialisation.
/// 
/// \param RETURN_TYPE        The return type of the function.
/// \param CALLING_CONVENTION The calling convention of the function (e.g. `__cdecl`, `__stdcall`, `__fastcall`, etc).
///                           This does not support optimised calling conventions, such as `__usercall` or `__userpurge`.
/// \param FUNCTION_NAME      The name of the function.
/// \param ADDRESS            The address of the function.
/// \param __VA_ARGS__        The parameters of the function.
/// 
/// \returns Use \ref GET_STATIC_HOOK_RESULT.
///
#define STATIC_HOOK(RETURN_TYPE, CALLING_CONVENTION, FUNCTION_NAME, ADDRESS, ...) \
    HOOK(RETURN_TYPE, CALLING_CONVENTION, FUNCTION_NAME, ADDRESS, __VA_ARGS__);   \
    __CMNLIB_INTERNAL_STATIC_HOOK_IMPL(FUNCTION_NAME, ADDRESS, INSTALL_HOOK)      \
    RETURN_TYPE CALLING_CONVENTION impl_##FUNCTION_NAME(__VA_ARGS__)

///
/// Gets the installation result of a hook defined with \ref STATIC_HOOK, \ref STATIC_THIS_HOOK, \ref STATIC_USER_HOOK or \ref STATIC_ASM_HOOK.
/// 
/// \param FUNCTION_NAME The name of the function that was hooked.
/// 
/// \returns `true` if the installation succeeeded. Otherwise, `false`.
///
#define GET_STATIC_HOOK_RESULT(FUNCTION_NAME) \
    result_##FUNCTION_NAME

#if defined(_M_AMD64) || defined(_M_IX86)

#if defined(_M_AMD64)
#define THIS_HOOK_PARAMS(CLASS_NAME) CLASS_NAME* self
#define THIS_HOOK_RETURN_PARAMS self
#elif defined(_M_IX86)
#define THIS_HOOK_PARAMS(CLASS_NAME) CLASS_NAME* self, void* _
#define THIS_HOOK_RETURN_PARAMS self, _
#endif

///
/// Defines the body of a hook for a class function in memory.
/// 
/// \param RETURN_TYPE        The return type of the function.
/// \param CLASS_NAME         The name of the class that contains the function being hooked.
///                           Use `void` if the class is undefined.
/// \param FUNCTION_NAME      The name of the function.
/// \param ADDRESS            The address of the function.
/// \param __VA_ARGS__        The parameters of the function (excluding `this`).
///
#define THIS_HOOK(RETURN_TYPE, CLASS_NAME, FUNCTION_NAME, ADDRESS, ...) \
    HOOK(RETURN_TYPE, __fastcall, FUNCTION_NAME, ADDRESS, THIS_HOOK_PARAMS(CLASS_NAME), __VA_ARGS__)

///
/// Defines the body of a hook for a class function in memory, and installs it upon initialisation.
/// 
/// \param RETURN_TYPE        The return type of the function.
/// \param CLASS_NAME         The name of the class that contains the function being hooked.
///                           Use `void` if the class is undefined.
/// \param FUNCTION_NAME      The name of the function.
/// \param ADDRESS            The address of the function.
/// \param __VA_ARGS__        The parameters of the function (excluding `this`).
///
#define STATIC_THIS_HOOK(RETURN_TYPE, CLASS_NAME, FUNCTION_NAME, ADDRESS, ...) \
    STATIC_HOOK(RETURN_TYPE, __fastcall, FUNCTION_NAME, ADDRESS, THIS_HOOK_PARAMS(CLASS_NAME), __VA_ARGS__)

#else
static_assert(false, "THIS_HOOK is not implemented for this architecture.");
#endif

///
/// Defines the body of a hook for a function in a virtual function table in memory.
/// 
/// \param RETURN_TYPE        The return type of the function.
/// \param CALLING_CONVENTION The calling convention of the function (e.g. `__cdecl`, `__stdcall`, `__fastcall`, etc).
///                           This does not support optimised calling conventions, such as `__usercall` or `__userpurge`.
/// \param CLASS_NAME         The name of the class that contains the function being hooked.
/// \param FUNCTION_NAME      The name of the function.
/// \param __VA_ARGS__        The parameters of the function.
///
#define VFTABLE_HOOK(RETURN_TYPE, CALLING_CONVENTION, CLASS_NAME, FUNCTION_NAME, ...) \
    HOOK(RETURN_TYPE, CALLING_CONVENTION, CLASS_NAME##_##FUNCTION_NAME, nullptr, __VA_ARGS__)

///
/// Defines the body of a hook for a function in a virtual function table in memory, and installs it upon initialisation.
/// 
/// \param RETURN_TYPE        The return type of the function.
/// \param CALLING_CONVENTION The calling convention of the function (e.g. `__cdecl`, `__stdcall`, `__fastcall`, etc).
///                           This does not support optimised calling conventions, such as `__usercall` or `__userpurge`.
/// \param CLASS_NAME         The name of the class that contains the function being hooked.
/// \param FUNCTION_NAME      The name of the function.
/// \param __VA_ARGS__        The parameters of the function.
/// 
/// \returns Use \ref GET_STATIC_VFTABLE_HOOK_RESULT.
///
#define STATIC_VFTABLE_HOOK(RETURN_TYPE, CALLING_CONVENTION, CLASS_NAME, FUNCTION_NAME, ...) \
    STATIC_HOOK(RETURN_TYPE, CALLING_CONVENTION, CLASS_NAME##_##FUNCTION_NAME, nullptr, __VA_ARGS__)

///
/// Gets the installation result of a hook defined with \ref STATIC_VFTABLE_HOOK.
/// 
/// \param CLASS_NAME    The name of the class that contains the function that was hooked.
/// \param FUNCTION_NAME The name of the function that was hooked.
/// 
/// \returns `true` if the installation succeeeded. Otherwise, `false`.
///
#define GET_STATIC_VFTABLE_HOOK_RESULT(CLASS_NAME, FUNCTION_NAME) \
    result_##CLASS_NAME##_##FUNCTION_NAME

#if defined(_M_AMD64)

///
/// Declares an x64 assembly hook. The body must be defined in an `*.asm` file using MASM.
/// 
/// This macro creates a scope for members of the hook to be declared in.
/// If no members are needed, close the scope immediately using empty braces.
/// 
/// \param NAME    The name of the hook.
/// \param ADDRESS The address to hook.
///
#define ASM_HOOK(NAME, ADDRESS)                         \
    extern "C" uint64_t x_##NAME = (uint64_t)(ADDRESS); \
    extern "C" uint64_t original_##NAME = x_##NAME;     \
    extern "C" void* impl_##NAME;                       \
    extern "C"

///
/// Declares an x64 assembly hook, and installs it upon initialisation. The body must be defined in an `*.asm` file using MASM.
/// 
/// This macro creates a scope for members of the hook to be declared in.
/// If no members are needed, close the scope immediately using empty braces.
/// 
/// \param NAME    The name of the hook.
/// \param ADDRESS The address to hook.
///
#define STATIC_ASM_HOOK(NAME, ADDRESS)                               \
    ASM_HOOK(NAME, ADDRESS) {}                                       \
    __CMNLIB_INTERNAL_STATIC_HOOK_IMPL(NAME, ADDRESS, INSTALL_HOOK); \
    extern "C"

#elif defined(_M_IX86)

///
/// Returns from an assembly hook to the original code.
/// 
/// \param NAME The name of the hook.
///
#define ASM_HOOK_RETURN(NAME) __asm jmp original_##NAME

///
/// Defines the body of an x86 assembly hook.
/// 
/// \param NAME    The name of the hook.
/// \param ADDRESS The address to hook.
///
#define ASM_HOOK(NAME, ADDRESS)        \
    void* x_##NAME = (void*)(ADDRESS); \
    void* original_##NAME = x_##NAME;  \
    void NAKED_FUNC impl_##NAME()

///
/// Defines the body of an x86 assembly hook, and installs it upon initialisation.
/// 
/// \param NAME    The name of the hook.
/// \param ADDRESS The address to hook.
///
#define STATIC_ASM_HOOK(NAME, ADDRESS)                               \
    void* x_##NAME = (void*)(ADDRESS);                               \
    void* original_##NAME = x_##NAME;                                \
    void impl_##NAME();                                              \
    __CMNLIB_INTERNAL_STATIC_HOOK_IMPL(NAME, ADDRESS, INSTALL_HOOK); \
    void NAKED_FUNC impl_##NAME()

#else
static_assert(false, "Assembly hooks are not implemented for this architecture.");
#endif

///
/// Installs a hook defined with \ref HOOK, \ref THIS_HOOK or \ref ASM_HOOK.
/// 
/// \param FUNCTION_NAME The name of the function to call before the original.
/// 
/// \returns `true` if the installation succeeeded, or if the hook was already installed. Otherwise, `false`.
///
#define INSTALL_HOOK(FUNCTION_NAME) \
    INSTALL_HOOK_EXPLICIT(FUNCTION_NAME, original_##FUNCTION_NAME)

///
/// Installs a hook defined with \ref HOOK, \ref THIS_HOOK or \ref ASM_HOOK at an explicit address.
/// 
/// \param FUNCTION_NAME The name of the function to call before the original.
/// \param ADDRESS       The address of the function to hook.
/// 
/// \returns `true` if the installation succeeeded, or if the hook was already installed. Otherwise, `false`.
///
#define INSTALL_HOOK_EXPLICIT(FUNCTION_NAME, ADDRESS)                           \
    std::invoke([&]()                                                           \
    {                                                                           \
        if (!original_##FUNCTION_NAME && !(ADDRESS))                            \
            return false;                                                       \
                                                                                \
        *(void**)&original_##FUNCTION_NAME = (void*)(ADDRESS);                  \
                                                                                \
        DetourTransactionBegin();                                               \
        DetourUpdateThread(GetCurrentThread());                                 \
        DetourAttach((void**)&original_##FUNCTION_NAME, &impl_##FUNCTION_NAME); \
                                                                                \
        return DetourTransactionCommit() == NO_ERROR;                           \
    })

///
/// Uninstalls a hook installed with \ref INSTALL_HOOK.
/// 
/// \param FUNCTION_NAME The name of the function to unhook.
/// 
/// \returns `true` if the uninstallation succeeeded, or if the hook was already uninstalled. Otherwise, `false`.
///
#define UNINSTALL_HOOK(FUNCTION_NAME)                                           \
    std::invoke([&]()                                                           \
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
/// \param CLASS_NAME     The name of the class that contains the function being hooked.
/// \param INSTANCE       A pointer to an instance of the class to extract the virtual function table pointer from.
/// \param FUNCTION_NAME  The name of the function to call before the original.
/// \param FUNCTION_INDEX The index of the function to hook.
/// 
/// \returns `true` if the installation succeeeded, or if the hook was already installed. Otherwise, `false`.
///
#define INSTALL_VFTABLE_HOOK(CLASS_NAME, INSTANCE, FUNCTION_NAME, FUNCTION_INDEX)                            \
    std::invoke([&]()                                                                                        \
    {                                                                                                        \
        if (original_##CLASS_NAME##_##FUNCTION_NAME)                                                         \
            return true;                                                                                     \
                                                                                                             \
        original_##CLASS_NAME##FUNCTION_NAME = (*(CLASS_NAME##_##FUNCTION_NAME***)INSTANCE)[FUNCTION_INDEX]; \
                                                                                                             \
        DetourTransactionBegin();                                                                            \
        DetourUpdateThread(GetCurrentThread());                                                              \
        DetourAttach((void**)&original##CLASS_NAME##_##FUNCTION_NAME, impl_##CLASS_NAME##_##FUNCTION_NAME);  \
                                                                                                             \
        return DetourTransactionCommit() == NO_ERROR;                                                        \
    })

///
/// Uninstalls a hook installed with \ref INSTALL_VFTABLE_HOOK.
/// 
/// \param CLASS_NAME    The name of the class that contains the function that was hooked.
/// \param FUNCTION_NAME The name of the function to unhook.
/// 
/// \returns `true` if the uninstallation succeeeded, or if the hook was already uninstalled. Otherwise, `false`.
///
#define UNINSTALL_VFTABLE_HOOK(CLASS_NAME, FUNCTION_NAME) \
    UNINSTALL_HOOK(CLASS_NAME##_##FUNCTION_NAME)

#ifdef _M_IX86
#include "HookingUserCall.h"
#endif
