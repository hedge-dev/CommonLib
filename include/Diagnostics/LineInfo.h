#pragma once

#include <filesystem>

namespace hedgedev::csl::diag
{
    struct LineInfo
    {
        ///
        /// The path to the source file containing the relevant code.
        ///
        std::filesystem::path SourceFilePath{};

        ///
        /// The address of the relevant code in memory.
        ///
        void* pAddress{};

        ///
        /// The row in the source file the relevant code is on.
        ///
        uint32_t Row{};

        ///
        /// The column in the source file the relevant code is on.
        ///
        uint32_t Column{};
    };
}
