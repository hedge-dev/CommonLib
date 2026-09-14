#pragma once

///
/// Marks a member to be imported from a dynamic link library.
///
#define DLL_IMPORT extern "C" __declspec(dllimport)

///
/// Loads a dynamic link library and imports a member from it.
///
/// \param LIBRARY_NAME The name of the dynamic link library to load.
/// \param MEMBER_NAME  The name of the exported member to import.
///
/// \returns A pointer to the imported member.
///          Use \ref FUNCTION_PTR to call imported function pointers.
///
#define DLL_IMPORT_EXPLICIT(LIBRARY_NAME, MEMBER_NAME) \
    GetProcAddress(LoadLibrary(TEXT(LIBRARY_NAME)), MEMBER_NAME)

///
/// Marks a member to be exported from a dynamic link library.
///
#define DLL_EXPORT extern "C" __declspec(dllexport)

#if defined(CMNLIB_X86) || defined(CMNLIB_ARM)

///
/// Marks a function as naked (no epilogue and prologue).
///
/// \remarks This specification is only available on x86 and ARM.
///
#define NAKED __declspec(naked)

#endif
