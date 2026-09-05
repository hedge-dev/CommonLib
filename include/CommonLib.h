#pragma once

#ifdef CMNLIB_NO_NAMESPACE_ALIASES
#define __CMNLIB_INTERNAL_MAKE_NAMESPACE_ALIAS(PARENT, NAME, ALIAS)
#else
#define __CMNLIB_INTERNAL_MAKE_NAMESPACE_ALIAS(PARENT, NAME, ALIAS) \
    namespace PARENT { namespace ALIAS = NAME; }
#endif

#if defined(_M_X64) || defined(_M_AMD64) || defined(__x86_64__)
#define CMNLIB_X64
#elif defined(_M_IX86) || defined(__i386__)
#define CMNLIB_X86
#elif defined(_M_ARM64) || defined(__aarch64__)
#define CMNLIB_ARM64
#elif defined(_M_ARM) || defined(__arm__)
#define CMNLIB_ARM
#endif

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#endif

#include <cstdint>

#include "Utility/Utility.h"
#include "Memory/Memory.h"

#include "Configuration/Configuration.h"
#include "Diagnostics/Diagnostics.h"
#include "Hooking/Hooking.h"

#if !defined(CMNLIB_HEADER_ONLY) && defined(__CMNLIB_INTERNAL_STATIC_LIB_ENROLMENT)
#pragma message("CommonLib must be statically linked. " __CMNLIB_INTERNAL_STATIC_LIB_ENROLMENT)
#pragma comment(lib, "CommonLib.lib")
#endif
