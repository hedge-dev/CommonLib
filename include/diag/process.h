#pragma once

#include <filesystem>
#include <optional>
#include <vector>

namespace hedgedev::csl::diag::process
{
    ///
    /// Starts a process.
    ///
    /// \tparam T_args The string type for the arguments.
    ///
    /// \param in_path The path to the executable to start.
    /// \param in_args The command line arguments to pass into the process.
    /// \param in_work The working directory of the process.
    ///
    /// \returns `true` if the process was started successfully. Otherwise, `false`.
    ///
    template <typename T_args>
    inline bool start(const std::filesystem::path& in_path, const std::vector<T_args>& in_args = {}, std::optional<std::filesystem::path> in_work = {});
}

#ifdef WIN32
#include "os/process_win32.inl"
#endif
