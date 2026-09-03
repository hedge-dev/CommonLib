#pragma once

#include <filesystem>
#include <vector>

#include "StackFrame.h"

namespace hedgedev::csl::diag::this_process
{
    ///
    /// Gets the command line arguments that were passed into this process at launch.
    ///
    template <typename TString>
    inline std::vector<TString> GetCommandLineArguments();

    ///
    /// Gets the path to the executable this process started from.
    ///
    inline std::filesystem::path GetExecutablePath();

    ///
    /// Gets the path to the directory containing executable this process started from.
    ///
    inline std::filesystem::path GetExecutableRoot();

    ///
    /// Gets the path to the module containing the specified address.
    ///
    /// \param in_pAddr The address to search for.
    ///
    inline std::filesystem::path GetModulePathFromAddress(const void* in_pAddr);

    ///
    /// Gets the working directory of this process.
    ///
    inline std::filesystem::path GetWorkingDirectory();

    ///
    /// Gets a stack trace.
    ///
    /// \param in_pContext  A pointer to a CPU context structure.
    /// \param in_maxFrames The maximum number of stack frames to capture.
    ///
    /// \returns A collection of stack frames.
    ///
    inline std::vector<StackFrame> GetStackTrace(const void* in_pContext, const size_t in_maxFrames = 0);

    ///
    /// Checks if this process is the owner of the specified address.
    ///
    /// \param in_pAddr The address to search for.
    ///
    inline bool HasAddress(const void* in_pAddr);

    ///
    /// Checks if a path is in the same directory as the executable this process started from.
    ///
    /// \param in_rPath The path to check.
    /// 
    /// \returns `true` if both paths are in the same directory. Otherwise, `false`.
    ///
    inline bool IsNeighbour(const std::filesystem::path& in_rPath);

    ///
    /// Restarts this process.
    ///
    /// \param in_rArgs The command line arguments to pass into the new process.
    /// 
    /// \returns `true` if this process was terminated successfully. Otherwise, `false`.
    ///
    template <typename TString>
    inline bool Restart(const std::vector<TString>& in_rArgs);

    ///
    /// Restarts this process.
    /// 
    /// \returns `true` if this process was terminated successfully. Otherwise, `false`.
    ///
    inline bool Restart();
}

#include "ThisProcess.inl"

#ifdef WIN32
#include "OS/Win32/ThisProcessWin32.inl"
#endif
