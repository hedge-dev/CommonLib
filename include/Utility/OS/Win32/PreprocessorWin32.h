#pragma once

#define IMPORT_FUNC(LIBRARY_NAME, FUNCTION_NAME) \
    GetProcAddress(LoadLibrary(TEXT(LIBRARY_NAME)), FUNCTION_NAME)

#define EXPORT_FUNC extern "C" __declspec(dllexport)

#define NAKED_FUNC __declspec(naked)
