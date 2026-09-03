#pragma once

#include <filesystem>

namespace hedgedev::csl::cfg::registry
{
    ///
    /// Reads a value from the registry.
    ///
    /// \param in_rPath      The path to the registry key containing the value.
    /// \param in_rValueName The name of the registry value.
    /// \param out_rData     The value to set.
    /// 
    /// \returns `true` if the value was read successfully. Otherwise, `false`.
    ///
    template <typename T>
    inline bool Read(const std::filesystem::path& in_rPath, const std::filesystem::path& in_rValueName, T& out_rData);

    ///
    /// Reads a value from the registry.
    ///
    /// \param in_rPath  The path to the registry value.
    /// \param out_rData The value to set.
    /// 
    /// \returns `true` if the value was read successfully. Otherwise, `false`.
    ///
    template <typename T>
    inline bool Read(const std::filesystem::path& in_rPath, T& out_rData);

    ///
    /// Writes a value to the registry.
    ///
    /// \param in_rPath      The path to the registry key containing the value.
    /// \param in_rValueName The name of the registry value.
    /// \param in_rData      The value to write.
    /// 
    /// \returns `true` if the value was written successfully. Otherwise, `false`.
    ///
    template <typename T>
    inline bool Write(const std::filesystem::path& in_rPath, const std::filesystem::path& in_rValueName, const T& in_rData);

    ///
    /// Writes a value to the registry.
    ///
    /// \param in_rPath The path to the registry value.
    /// \param in_rData The value to write.
    /// 
    /// \returns `true` if the value was written successfully. Otherwise, `false`.
    ///
    template <typename T>
    inline bool Write(const std::filesystem::path& in_rPath, const T& in_rData);
}

#ifdef WIN32
#include "OS/Win32/RegistryWin32.inl"
#endif
