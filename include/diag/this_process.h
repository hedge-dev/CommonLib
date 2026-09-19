#pragma once

#include <filesystem>
#include <vector>

#include "diag/stack_frame.h"
#include "ut/expr/string_types.h"

namespace hedgedev::csl::diag::this_process
{
    ///
    /// Gets the command line arguments that were passed into this process at launch.
    ///
    template <ut::expr::any_string_t T>
    inline std::vector<T> get_command_line();

    ///
    /// Gets the path to the executable this process started from.
    ///
    inline std::filesystem::path get_executable_path();

    ///
    /// Gets the path to the directory containing executable this process started from.
    ///
    inline std::filesystem::path get_executable_root();

    ///
    /// Gets the path to the module containing the specified address.
    ///
    /// \param in_address The address to search for.
    ///
    inline std::filesystem::path get_module_path_from_address(const void* in_address);

    ///
    /// Gets the working directory of this process.
    ///
    inline std::filesystem::path get_working_directory();

    ///
    /// Gets a stack trace.
    ///
    /// \param in_context    A pointer to a CPU context structure.
    /// \param in_max_frames The maximum number of stack frames to capture.
    ///
    /// \returns A collection of stack frames.
    ///
    inline std::vector<stack_frame> get_stack_trace(const void* in_context, const size_t in_max_frames = 0);

    ///
    /// Checks if this process is the owner of the specified address.
    ///
    /// \param in_address The address to search for.
    ///
    /// \returns `true` if the address is inside this process' memory.
    ///          Otherwise, `false`.
    ///
    inline bool has_address(const void* in_address);

    ///
    /// Checks if a path is in the same directory as the executable this process
    /// started from.
    ///
    /// \param in_path The path to check.
    ///
    /// \returns `true` if both paths are in the same directory. Otherwise, `false`.
    ///
    inline bool is_neighbour(const std::filesystem::path& in_path);

    ///
    /// Restarts this process.
    ///
    /// \tparam T The string type for the arguments.
    ///
    /// \param in_args The command line arguments to pass into the new process.
    ///
    /// \returns `true` if this process was terminated successfully. Otherwise, `false`.
    ///
    template <ut::expr::any_string_t T>
    inline bool restart(const std::vector<T>& in_args);

    ///
    /// Restarts this process.
    ///
    /// \returns `true` if this process was terminated successfully. Otherwise, `false`.
    ///
    inline bool restart();
}

#include "this_process.inl"

#ifdef WIN32
#include "os/this_process_win32.inl"
#endif
