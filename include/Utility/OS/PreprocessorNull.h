#pragma once

#define IMPORT_FUNC(LIBRARY_NAME, FUNCTION_NAME) \
    static_assert(false, "IMPORT_FUNC is not implemented for this OS.");

#define EXPORT_FUNC \
    static_assert(false, "EXPORT_FUNC is not implemented for this OS.");

#define NAKED_FUNC \
    static_assert(false, "NAKED_FUNC is not implemented for this OS.");
