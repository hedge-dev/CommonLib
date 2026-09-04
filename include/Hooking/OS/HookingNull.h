///
/// \file HookingNull.h
/// 
/// Macros for managing hooks without implementation.
///

#pragma once

#define FUNCTION_PTR(RETURN_TYPE, CALLING_CONVENTION, FUNCTION_NAME, ADDRESS, ...) \
    static_assert(false, "FUNCTION_PTR is not implemented for this OS.");

#define IMPORT_FUNCTION_PTR(RETURN_TYPE, LIBRARY_NAME, FUNCTION_NAME, ...) \
    static_assert(false, "IMPORT_FUNCTION_PTR is not implemented for this OS.");

#define HOOK(RETURN_TYPE, CALLING_CONVENTION, FUNCTION_NAME, ADDRESS, ...) \
    static_assert(false, "HOOK is not implemented for this OS.");

#define STATIC_HOOK(RETURN_TYPE, CALLING_CONVENTION, FUNCTION_NAME, ADDRESS, ...) \
    static_assert(false, "STATIC_HOOK is not implemented for this OS.");

#define GET_STATIC_HOOK_RESULT(FUNCTION_NAME) \
    static_assert(false, "GET_STATIC_HOOK_RESULT is not implemented for this OS.");

#define VFTABLE_HOOK(RETURN_TYPE, CALLING_CONVENTION, CLASS_NAME, FUNCTION_NAME, ...) \
    static_assert(false, "VFTABLE_HOOK is not implemented for this OS.");

#define STATIC_VFTABLE_HOOK(RETURN_TYPE, CALLING_CONVENTION, CLASS_NAME, FUNCTION_NAME, ...) \
    static_assert(false, "STATIC_VFTABLE_HOOK is not implemented for this OS.");

#define GET_STATIC_VFTABLE_HOOK_RESULT(CLASS_NAME, FUNCTION_NAME) \
    static_assert(false, "GET_STATIC_VFTABLE_HOOK_RESULT is not implemented for this OS.");

#define USER_FUNCTION_PTR(RETURN_TYPE, CALLING_CONVENTION, FUNCTION_NAME, ADDRESS, REGISTERS, PARAM_COUNT, ...) \
    static_assert(false, "USER_FUNCTION_PTR is not implemented for this OS.");

#define USER_HOOK(RETURN_TYPE, CALLING_CONVENTION, FUNCTION_NAME, ADDRESS, REGISTERS, PARAM_COUNT, ...) \
    static_assert(false, "USER_HOOK is not implemented for this OS.");

#define STATIC_USER_HOOK(RETURN_TYPE, CALLING_CONVENTION, FUNCTION_NAME, ADDRESS, REGISTERS, PARAM_COUNT, ...) \
    static_assert(false, "STATIC_USER_HOOK is not implemented for this OS.");

#define ASM_HOOK_RETURN(NAME) \
    static_assert(false, "ASM_HOOK_RETURN is not implemented for this OS.");

#define ASM_HOOK(NAME, ADDRESS) \
    static_assert(false, "ASM_HOOK is not implemented for this OS.");

#define STATIC_ASM_HOOK(NAME, ADDRESS) \
    static_assert(false, "STATIC_ASM_HOOK is not implemented for this OS.");

#define INSTALL_HOOK(FUNCTION_NAME) \
    static_assert(false, "INSTALL_HOOK is not implemented for this OS.");

#define INSTALL_HOOK_EXPLICIT(FUNCTION_NAME, ADDRESS) \
    static_assert(false, "INSTALL_HOOK_EXPLICIT is not implemented for this OS.");

#define UNINSTALL_HOOK(FUNCTION_NAME) \
    static_assert(false, "UNINSTALL_HOOK is not implemented for this OS.");

#define INSTALL_VFTABLE_HOOK(CLASS_NAME, OBJECT, FUNCTION_NAME, FUNCTION_INDEX) \
    static_assert(false, "INSTALL_VFTABLE_HOOK is not implemented for this OS.");

#define UNINSTALL_VFTABLE_HOOK(CLASS_NAME, FUNCTION_NAME) \
    static_assert(false, "UNINSTALL_VFTABLE_HOOK is not implemented for this OS.");

#define INSTALL_USER_HOOK(FUNCTION_NAME) \
    static_assert(false, "INSTALL_USER_HOOK is not implemented for this OS.");

#define INSTALL_USER_HOOK_EXPLICIT(FUNCTION_NAME, ADDRESS) \
    static_assert(false, "INSTALL_USER_HOOK_EXPLICIT is not implemented for this OS.");

#define UNINSTALL_USER_HOOK(FUNCTION_NAME) \
    static_assert(false, "UNINSTALL_USER_HOOK is not implemented for this OS.");
