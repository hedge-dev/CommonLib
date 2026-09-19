#pragma once

#include <filesystem>
#include <optional>

#include "ut/expr/string_types.h"

namespace hedgedev::csl::ut::env
{
    ///
    /// Gets a file in the system environment.
    ///
    /// \param in_filename The file to search for.
    ///
    /// \returns The path to the file, if found.
    ///
    inline std::optional<std::filesystem::path> get_file(const std::filesystem::path& in_filename);

    ///
    /// Gets a variable in the system environment.
    ///
    /// \tparam T_result  The value type to parse.
    /// \tparam T_name The name string type.
    ///
    /// \param in_name          The variable to search for.
    /// \param in_default_value The default value to use, if the variable could not
    ///                         be found.
    ///
    /// \returns The value of the variable, if found.
    ///
    template<typename T_result, expr::any_string_t T_name>
    inline std::optional<T_result> get_variable(const T_name& in_name, std::optional<T_result> in_default_value = std::nullopt);
}

#ifdef WIN32
#include "os/env_win32.inl"
#endif
