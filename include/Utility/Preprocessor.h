#pragma once

///
/// Deletes an object and resets its pointer.
///
/// \param PTR The pointer to the object to delete.
///
#define SAFE_DELETE(PTR) \
    if (PTR) { delete PTR; PTR = nullptr; }

///
/// Returns a value if the condition is not met.
///
/// \param CONDITION    The condition to check.
/// \param RETURN_VALUE The value to return.
///
#define ASSERT_RETURN(CONDITION, RETURN_VALUE) \
    if (!(CONDITION)) return RETURN_VALUE;

///
/// Returns `true` if the condition is not met.
///
/// \param CONDITION The condition to check.
///
#define ASSERT_RETURN_TRUE(CONDITION) \
    ASSERT_RETURN(CONDITION, true)

///
/// Returns `false` if the condition is not met.
///
/// \param CONDITION The condition to check.
///
#define ASSERT_RETURN_FALSE(CONDITION) \
    ASSERT_RETURN(CONDITION, false)

#ifdef WIN32
#include "Win32/Preprocessor.h"
#endif
