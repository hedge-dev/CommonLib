#pragma once

#include <filesystem>

namespace hedgedev::csl::cfg::registry
{
    ///
    /// Reads a value from the registry.
    ///
    /// \param in_path       The path to the registry key containing the value.
    /// \param in_value_name The name of the registry value.
    /// \param out_result    The value to set.
    ///
    /// \returns `true` if the value was read successfully. Otherwise, `false`.
    ///
    template <typename T>
    inline bool read(const std::filesystem::path& in_path, const std::filesystem::path& in_value_name, T& out_result);

    ///
    /// Reads a value from the registry.
    ///
    /// \param in_path    The path to the registry value.
    /// \param out_result The value to set.
    ///
    /// \returns `true` if the value was read successfully. Otherwise, `false`.
    ///
    template <typename T>
    inline bool read(const std::filesystem::path& in_path, T& out_result);

    ///
    /// Writes a value to the registry.
    ///
    /// \param in_path       The path to the registry key containing the value.
    /// \param in_value_name The name of the registry value.
    /// \param in_value      The value to write.
    ///
    /// \returns `true` if the value was written successfully. Otherwise, `false`.
    ///
    template <typename T>
    inline bool write(const std::filesystem::path& in_path, const std::filesystem::path& in_value_name, const T& in_value);

    ///
    /// Writes a value to the registry.
    ///
    /// \param in_path  The path to the registry value.
    /// \param in_value The value to write.
    ///
    /// \returns `true` if the value was written successfully. Otherwise, `false`.
    ///
    template <typename T>
    inline bool write(const std::filesystem::path& in_path, const T& in_value);
}

#ifdef WIN32
#include "os/registry_win32.inl"
#endif
