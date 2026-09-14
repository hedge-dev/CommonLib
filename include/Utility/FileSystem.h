#pragma once

#include <filesystem>

namespace hedgedev::csl::ut::filesystem
{
    ///
    /// Checks if a path is in the same directory as another.
    ///
    /// \param in_rPathA The first path.
    /// \param in_rPathB The second path.
    /// 
    /// \returns `true` if both paths are in the same directory. Otherwise, `false`.
    ///
    inline bool IsNeighbour(const std::filesystem::path& in_rPathA, const std::filesystem::path& in_rPathB);

    ///
    /// Truncates specific file types in a directory down to a maximum number of files.
    ///
    /// \param in_rPath     The directory containing the files to truncate.
    /// \param in_extension The extension of the files to truncate.
    /// \param in_max       The maximum number of files of the specified type that can exist in this directory.
    /// \param in_rrCompare The comparison method that determines how the files to be truncated should be sorted.
    ///
    template <typename T>
    inline std::error_code TruncateFiles(const std::filesystem::path& in_rPath, std::string_view in_extension, size_t in_max, T&& in_rrCompare);

    ///
    /// Truncates specific file types in a directory down to a maximum number of files, sorted by age.
    ///
    /// \param in_rPath     The directory containing the files to truncate.
    /// \param in_extension The extension of the files to truncate.
    /// \param in_max       The maximum number of files of the specified type that can exist in this directory.
    ///
    inline std::error_code TruncateFilesByAge(const std::filesystem::path& in_rPath, std::string_view in_extension, size_t in_max = 10);
}

#include "FileSystem.inl"

__CMNLIB_INTERNAL_MAKE_NAMESPACE_ALIAS(hedgedev::csl::ut, filesystem, fs);
