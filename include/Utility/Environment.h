#pragma once

#include <filesystem>
#include <optional>

namespace hedgedev::csl::ut::env
{
    ///
    /// Gets a file in the system environment.
    ///
    /// \param in_rFileName The file to search for.
    /// 
    /// \returns The path to the file, if found.
    ///
    inline std::optional<std::filesystem::path> GetFile(const std::filesystem::path& in_rFileName);

    ///
    /// Gets a variable in the system environment.
    ///
    /// \tparam TOut  The value type to parse.
    /// \tparam TName The name string type.
    /// 
    /// \param in_rName        The variable to search for.
    /// \param in_defaultValue The default value to use, if the variable could not be found.
    /// 
    /// \returns The value of the variable, if found.
    ///
    template<typename TOut, expr::AnyString TName>
    inline std::optional<TOut> GetVariable(const TName& in_rName, std::optional<TOut> in_defaultValue = std::nullopt);
}

#ifdef WIN32
#include "Win32/Environment.inl"
#endif
