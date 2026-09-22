#pragma once

#include "ut/expr/string_types.h"

namespace hedgedev::csl::host::clipboard
{
    ///
    /// Sets the clipboard text.
    ///
    /// \param in_value The value to write to the clipboard.
    ///
    /// \returns `true` if the clipboard was written to successfully.
    ///          Otherwise, `false`.
    ///
    template <typename T>
    inline bool set_text(const T& in_value);
}

#ifdef WIN32
#include "os/clipboard_win32.inl"
#endif
