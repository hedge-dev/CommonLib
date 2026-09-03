#pragma once

///
/// Deletes an object and resets its pointer.
///
/// \param PTR The pointer to the object to delete.
///
#define SAFE_DELETE(PTR) \
    if (PTR) { delete PTR; PTR = nullptr; }

#ifdef WIN32
#include "OS/Win32/PreprocessorWin32.h"
#else
#include "OS/PreprocessorNull.h"
#endif
