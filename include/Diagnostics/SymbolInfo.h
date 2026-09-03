#pragma once

#include <string>

namespace hedgedev::csl::diag
{
    struct SymbolInfo
    {
        std::string Name{};
        void* pAddress{};
        uint32_t Flags{};
        size_t Displacement{};
    };
}
