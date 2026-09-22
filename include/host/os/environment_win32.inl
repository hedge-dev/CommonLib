#include <filesystem>
#include <optional>
#include <string>

#include "host/common.h"
#include "ut/expr/string_types.h"

namespace hedgedev::csl::host::environment
{
	inline std::optional<std::filesystem::path> get_file(const std::filesystem::path& in_filename)
	{
		std::filesystem::path result{};

		wchar_t buffer[MAX_PATH]{};

		if (_wsearchenv_s(in_filename.c_str(), L"PATH", buffer, ARRAYSIZE(buffer)) == 0 && *buffer != 0)
			result = buffer;

		return result;
	}
	
    template<typename T_result, ut::expr::any_string_t T_name>
    inline std::optional<T_result> get_variable(const T_name& in_name, std::optional<T_result> in_default_value)
    {
		std::optional<T_result> result{};

        std::wstring name{};

		if constexpr (ut::expr::any_string_w_t<T_name>)
		{
			name = in_name;
		}
		else
		{
			if (!ut::string::try_convert<std::wstring>(in_name, name))
				return result = in_default_value;
		}

		if (auto buffer_size = GetEnvironmentVariableW(name.c_str(), NULL, 0))
		{
			std::wstring value_str{};
			value_str.resize(buffer_size + sizeof(wchar_t));

			buffer_size = GetEnvironmentVariableW(name.c_str(), value_str.data(), buffer_size);

			if (!buffer_size || !ut::string::try_parse(value_str, result))
				result = in_default_value;
		}
		else
		{
			result = in_default_value;
		}

        return result;
    }
}
