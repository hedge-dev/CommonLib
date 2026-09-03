#pragma once

#include <filesystem>
#include <optional>
#include <vector>

namespace hedgedev::csl::diag::process
{
    ///
    /// Starts a process.
    ///
    /// \param in_rPath The path to the executable to start.
    /// \param in_rArgs The command line arguments to pass into the process.
    /// \param in_work  The working directory of the process.
    /// 
    /// \returns `true` if the process was started successfully. Otherwise, `false`.
    ///
    template <typename TString>
    inline bool Start(const std::filesystem::path& in_rPath, const std::vector<TString>& in_rArgs = {}, std::optional<std::filesystem::path> in_work = {});
}

#ifdef WIN32
#include "OS/Win32/ProcessWin32.inl"
#endif
