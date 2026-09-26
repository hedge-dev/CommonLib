#include <sstream>

#include "host/common.h"
#include "ut/expr/string_expr.h"
#include "ut/encoding.h"

namespace hedgedev::csl::host::clipboard
{
    template <typename T>
    inline bool set_text(const T& in_value)
    {
        std::wstringstream stream{};

        if constexpr (ut::expr::any_string<T>)
        {
            std::wstring str{};

            if (!ut::encoding::try_convert<std::wstring>(in_value, str))
                return false;

            stream << str;
        }
        else
        {
            stream << in_value;
        }

		const auto data_str = stream.str();
		const auto data_len = (data_str.size() * sizeof(WCHAR)) + sizeof(WCHAR);

		auto buffer_handle = GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, data_len);

		if (!buffer_handle)
			return false;

		auto buffer = (LPWSTR)GlobalLock(buffer_handle);

		if (!buffer)
		{
			GlobalFree(buffer_handle);
			return false;
		}

		memcpy(buffer, data_str.c_str(), data_len);

		GlobalUnlock(buffer_handle);

		if (!OpenClipboard(NULL))
		{
			GlobalFree(buffer_handle);
			return false;
		}

		if (!EmptyClipboard() || !SetClipboardData(CF_UNICODETEXT, buffer_handle))
		{
			CloseClipboard();
			GlobalFree(buffer_handle);
			return false;
		}

		CloseClipboard();

		return true;
    }
}
