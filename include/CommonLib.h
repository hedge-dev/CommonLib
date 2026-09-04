#pragma once

#ifdef CMNLIB_NO_NAMESPACE_ALIASES
#define __CMNLIB_INTERNAL_MAKE_NAMESPACE_ALIAS(PARENT, NAME, ALIAS)
#else
#define __CMNLIB_INTERNAL_MAKE_NAMESPACE_ALIAS(PARENT, NAME, ALIAS) \
    namespace PARENT { namespace ALIAS = NAME; }
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
