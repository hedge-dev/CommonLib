#pragma once

#include <filesystem>

namespace hedgedev::csl::ut::filesystem
{
    ///
    /// Checks if a path is in the same directory as another.
    ///
    /// \param in_left  The first path.
    /// \param in_right The second path.
    ///
    /// \returns `true` if both paths are in the same directory. Otherwise, `false`.
    ///
    inline bool is_neighbour(const std::filesystem::path& in_left, const std::filesystem::path& in_right);

    ///
    /// Truncates specific file types in a directory down to a maximum number of files.
    ///
    /// \param in_path         The directory containing the files to truncate.
    /// \param in_extension    The extension of the files to truncate.
    /// \param in_max          The maximum number of files of the specified type that
    ///                        can exist in this directory.
    /// \param in_compare_func The comparison method that determines how the files to be
    ///                        truncated should be sorted.
    ///
    template <typename T>
    inline std::error_code truncate_files(const std::filesystem::path& in_path, std::string_view in_extension, size_t in_max, T&& in_compare_func);

    ///
    /// Truncates specific file types in a directory down to a maximum number of files,
    /// sorted by age.
    ///
    /// \param in_path      The directory containing the files to truncate.
    /// \param in_extension The extension of the files to truncate.
    /// \param in_max       The maximum number of files of the specified type that can
    ///                     exist in this directory.
    ///
    inline std::error_code truncate_files_by_age(const std::filesystem::path& in_path, std::string_view in_extension, size_t in_max = 10);
}

#include "filesystem.inl"

__CMNLIB_INTERNAL_MAKE_NAMESPACE_ALIAS(hedgedev::csl::ut, filesystem, fs);
