#pragma once

#include <filesystem>

namespace hedgedev::csl::diag
{
    struct line_info
    {
        ///
        /// The path to the source file containing the relevant code.
        ///
        std::filesystem::path source_file_path{};

        ///
        /// The address of the relevant code in memory.
        ///
        void* address{};

        ///
        /// The row in the source file the relevant code is on.
        ///
        uint32_t row{};

        ///
        /// The column in the source file the relevant code is on.
        ///
        uint32_t column{};
    };
}
