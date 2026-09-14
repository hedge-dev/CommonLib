namespace hedgedev::csl::ut::env
{
	inline std::optional<std::filesystem::path> GetFile(const std::filesystem::path& in_rFileName)
	{
		std::filesystem::path result{};

		wchar_t buffer[MAX_PATH]{};
		_wsearchenv_s(in_rFileName.c_str(), L"PATH", buffer, ARRAYSIZE(buffer));

		if (*buffer != 0)
			result = buffer;

		return result;
	}
	
    template<typename TOut, expr::AnyString TName>
    inline std::optional<TOut> GetVariable(const TName& in_rName, std::optional<TOut> in_defaultValue)
    {
        std::wstring name{};

		if constexpr (expr::WideString<TName>)
		{
			name = in_rName;
		}
		else
		{
			if (!string::TryConvert<std::wstring>(in_rName, name))
				return in_defaultValue;
		}

        const auto bufferSize = GetEnvironmentVariableW(name.c_str(), NULL, 0);

        if (!bufferSize)
            return in_defaultValue;

        std::wstring valueStr{};
        TOut value{};

        valueStr.resize(bufferSize + sizeof(wchar_t));

        GetEnvironmentVariableW(name.c_str(), valueStr.data(), bufferSize);

        if (!string::TryParse(valueStr, value))
            return in_defaultValue;

        return value;
    }
}
