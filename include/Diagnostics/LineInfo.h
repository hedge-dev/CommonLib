#pragma once

#include <filesystem>

namespace hedgedev::csl::diag
{
    struct LineInfo
    {
        std::filesystem::path SourceFilePath{};
        void* pAddress{};
        uint32_t Row{};
        uint32_t Column{};
    };
}
