namespace hedgedev::csl::ut::filesystem
{
	inline std::filesystem::path FindInEnvironment(const std::filesystem::path& in_rFileName)
	{
		std::filesystem::path result{};

		wchar_t buffer[MAX_PATH]{};
		_wsearchenv_s(in_rFileName.c_str(), L"PATH", buffer, ARRAYSIZE(buffer));

		if (*buffer != '\0')
			result = buffer;

		return result;
	}
}
