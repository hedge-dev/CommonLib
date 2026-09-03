namespace hedgedev::csl::diag::process
{
	template <typename TString = std::wstring_view>
	inline bool Start(const std::filesystem::path& in_rPath, const std::vector<TString>& in_rArgs, std::optional<std::filesystem::path> in_work)
	{
		if (in_rPath.empty())
			return false;

		if (!in_work.has_value())
			in_work = in_rPath.parent_path();

		std::wstringstream commandLine{};

		commandLine << in_rPath.wstring();

		for (const auto& rArg : in_rArgs)
			commandLine << L' ' << rArg;

		STARTUPINFOW startInfo{ sizeof(STARTUPINFOW) };
		PROCESS_INFORMATION procInfo{};

		const auto commandLineW = commandLine.str();
		const auto pathW = in_rPath.wstring();
		const auto workW = in_work.value().wstring();

		if (!CreateProcessW(pathW.c_str(), (LPWSTR)commandLineW.c_str(), NULL, NULL, FALSE, 0, NULL, workW.c_str(), &startInfo, &procInfo))
			return false;

		if (procInfo.hProcess)
			CloseHandle(procInfo.hProcess);

		if (procInfo.hThread)
			CloseHandle(procInfo.hThread);

		return true;
	}
}
