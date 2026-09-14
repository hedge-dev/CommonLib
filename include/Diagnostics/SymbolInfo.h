#pragma once

#include <string>

namespace hedgedev::csl::diag
{
    struct SymbolInfo
    {
        ///
        /// The name of this symbol.
        ///
        std::string Name{};

        ///
        /// The address of this symbol in memory.
        ///
        void* pAddress{};

        ///
        /// The flags pertaining to this symbol.
        ///
        uint32_t Flags{};

        ///
        /// The offset of the program counter inside this symbol.
        ///
        size_t Displacement{};
    };
}
