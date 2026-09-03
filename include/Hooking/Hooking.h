#pragma once

#ifndef CMNLIB_NO_HOOK_MACROS

#ifdef WIN32
#include "OS/Win32/HookingWin32.h"
#else
#include "OS/HookingNull.h"
#endif

#endif // CMNLIB_NO_HOOK_MACROS
