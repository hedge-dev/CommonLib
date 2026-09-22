#pragma once

#include <filesystem>
#include <optional>

#include "ut/expr/string_types.h"

namespace hedgedev::csl::host::environment
{
    ///
    /// Gets a file in the host environment.
    ///
    /// \param in_filename The file to search for.
    ///
    /// \returns The path to the file, if found.
    ///
    inline std::optional<std::filesystem::path> get_file(const std::filesystem::path& in_filename);

    ///
    /// Gets a variable in the host environment.
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
    template<typename T_result, ut::expr::any_string_t T_name>
    inline std::optional<T_result> get_variable(const T_name& in_name, std::optional<T_result> in_default_value = std::nullopt);
}

#ifdef WIN32
#include "os/environment_win32.inl"
#endif

__CMNLIB_INTERNAL_MAKE_NAMESPACE_ALIAS(hedgedev::csl::host, environment, env);
