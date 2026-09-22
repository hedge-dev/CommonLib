#include <filesystem>
#include <optional>
#include <vector>

#include "host/common.h"

namespace hedgedev::csl::diag::process
{
	template <typename T_args = std::wstring_view>
	inline bool start(const std::filesystem::path& in_path, const std::vector<T_args>& in_args, std::optional<std::filesystem::path> in_work)
	{
		if (in_path.empty())
			return false;

		if (!in_work.has_value())
			in_work = in_path.parent_path();

		std::wstringstream command_line{};

		command_line << in_path.wstring();

		for (const auto& arg : in_args)
			command_line << L' ' << arg;

		STARTUPINFOW start_info{ sizeof(STARTUPINFOW) };
		PROCESS_INFORMATION proc_info{};

		const auto command_line_w = command_line.str();
		const auto path_w = in_path.wstring();
		const auto work_w = in_work.value().wstring();

		if (!CreateProcessW(path_w.c_str(), (LPWSTR)command_line_w.c_str(), NULL, NULL, FALSE, 0, NULL, work_w.c_str(), &start_info, &proc_info))
			return false;

		if (proc_info.hProcess)
			CloseHandle(proc_info.hProcess);

		if (proc_info.hThread)
			CloseHandle(proc_info.hThread);

		return true;
	}
}
