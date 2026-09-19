#pragma once

#include <string>

namespace hedgedev::csl::diag
{
    struct symbol_info
    {
        ///
        /// The name of this symbol.
        ///
        std::string name{};

        ///
        /// The address of this symbol in memory.
        ///
        void* address{};

        ///
        /// The flags pertaining to this symbol.
        ///
        uint32_t flags{};

        ///
        /// The offset of the program counter inside this symbol.
        ///
        uint64_t displacement{};
    };
}
