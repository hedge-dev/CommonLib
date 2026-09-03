#include <charconv>
#include <iomanip>
#include <regex>

namespace hedgedev::csl::ut::string
{
	template <typename TLiteralA, typename TLiteralB>
	inline bool Compare(const TLiteralA* in_a, const TLiteralB* in_b, bool in_isCaseSensitive)
	{
		using TStringViewA = std::basic_string_view<TLiteralA>;
		using TStringViewB = std::basic_string_view<TLiteralB>;

		const auto viewA = TStringViewA(in_a);
		const auto viewB = TStringViewB(in_b);

		if (viewA.length() != viewB.length())
			return false;

		if constexpr (sizeof(TLiteralA) < sizeof(TLiteralB))
		{
			std::basic_string<TLiteralB> cmpA{};

			if (TryConvert(in_a, cmpA))
			{
				if (in_isCaseSensitive)
				{
					return cmpA == viewB;
				}
				else
				{
					return ToLower(TStringViewB(cmpA)) == ToLower(viewB);
				}
			}

			return false;
		}
		else if constexpr (sizeof(TLiteralA) > sizeof(TLiteralB))
		{
			std::basic_string<TLiteralA> cmpB{};

			if (TryConvert(in_b, cmpB))
			{
				if (in_isCaseSensitive)
				{
					return viewA == cmpB;
				}
				else
				{
					return ToLower(viewA) == ToLower(TStringViewA(cmpB));
				}
			}

			return false;
		}
		else if constexpr (!std::is_same_v<std::remove_cv_t<TLiteralA>, std::remove_cv_t<TLiteralB>>)
		{
			// The strings use the same char length, but are encoded differently.
			static_assert(false, "Unconvertable mismatching string types.");
		}

		const auto bAsViewA = TStringViewA((const TLiteralA*)in_b);

		if (in_isCaseSensitive)
			return viewA == bAsViewA;

		return ToLower(viewA) == ToLower(bAsViewA);
	}

	template <typename TStringView, typename TLiteral>
	inline bool Contains(TStringView in_str, const TLiteral* in_pSubStr)
	{
		return in_str.find(in_pSubStr) != TStringView::npos;
	}

    template <typename TDst, typename TSrc>
    inline TDst Convert(TSrc&& in_rrStr)
	{
		TDst result{};
		TryConvert<TDst>(in_rrStr, result);
		return result;
	}

	template <typename TString, typename TLiteral>
	inline constexpr TString CreateInferredString(const TLiteral* in_pStr)
	{
		auto result = std::basic_string_view<TLiteral>(in_pStr);

		return TString(result.begin(), result.end());
	}

	template <typename TString, typename TLiteral, typename TStringView>
	static TString Escape(TStringView in_str, TLiteral in_charToEscape, TLiteral in_escapeChar)
	{
		std::basic_stringstream<TLiteral> result{};

		for (const auto& c : in_str)
		{
			if (c == in_charToEscape)
				result << in_escapeChar;

			result << c;
		}

		return result.str();
	}

	template <typename TLiteral, typename TString>
	inline TString Format(const TLiteral* in_pStr, ...)
	{
		va_list args;
		va_start(args, in_pStr);

		size_t size{};

		if constexpr (std::is_same_v<TLiteral, char>)
		{
			size = _vscprintf(in_pStr, args);
		}
		else if constexpr (std::is_same_v<TLiteral, wchar_t>)
		{
			size = _vscwprintf(in_pStr, args);
		}
		else
		{
			static_assert(false, "Unsupported string type.");
		}

		auto buffer = std::vector<TLiteral>(size + 1);

		if (size > 0)
		{
			if constexpr (std::is_same_v<TLiteral, char>)
			{
				_vsnprintf_s(buffer.data(), buffer.size(), _TRUNCATE, in_pStr, args);
			}
			else if constexpr (std::is_same_v<TLiteral, wchar_t>)
			{
				_vsnwprintf_s(buffer.data(), buffer.size(), _TRUNCATE, in_pStr, args);
			}
		}

		va_end(args);

		return TString(buffer.data(), size);
	}

	template <typename TStringView>
	inline size_t GetWidth(TStringView in_str)
	{
		size_t result{};

		std::basic_stringstream<typename TStringView::value_type> stream(in_str);
		std::basic_string<typename TStringView::value_type> line{};

		while (std::getline(stream, line))
			result = std::max(result, line.length());

		return result;
	}

	template <typename TStringView, typename TString>
	static TString Hyperlink(TStringView in_str, TStringView in_url)
	{
		if (in_url.empty())
			return in_str;

		std::basic_stringstream<typename TString::value_type> result{};

		result << CreateInferredString<TString>("<a href=\"")
			<< in_url << CreateInferredString<TString>("\">")
			<< in_str << CreateInferredString<TString>("</a>");

		return result.str();
	}

	template <typename TLiteral, typename TString>
	inline TString Join(const TLiteral* in_pDelimiter, const std::vector<TString>& in_rStrings)
	{
		std::basic_stringstream<typename TString::value_type> result{};

		if (in_rStrings.empty())
			return result.str();

		const auto length = in_rStrings.size();

		for (auto i = 0; i < length; i++)
		{
			const auto& rStr = in_rStrings[i];

			result << rStr;

			if (i == length - 1)
				continue;

			result << in_pDelimiter;
		}

		return result.str();
	}

	template <typename TStringView, typename TLiteral, typename TString>
	inline TString Pad(TStringView in_str, const TLiteral in_padChar)
	{
		std::basic_stringstream<TLiteral> result{};

		result << in_padChar << in_str << in_padChar;

		return result.str();
	}

	template <typename TStringView, typename TString>
	inline TString RemoveHtmlTags(TStringView in_str)
	{
		return std::regex_replace(in_str,
			std::basic_regex<typename TStringView::value_type>(CreateInferredString<TStringView>("<[^>]*>")),
			CreateInferredString<TStringView>(""));
	}

	template <typename TStringView, typename TLiteral, typename TString>
	inline std::vector<TString> Split(TStringView in_str, const TLiteral* in_pDelimiter)
	{
		std::vector<TString> result{};
		size_t start{};

		const auto delimiterLength = std::char_traits<TLiteral>::length(in_pDelimiter);

		while (true)
		{
			const auto pos = in_str.find(in_pDelimiter, start);
			const auto token = in_str.substr(start, pos == TStringView::npos ? in_str.size() - start : pos - start);

			result.emplace_back(Trim(token));

			if (pos == TStringView::npos)
				break;

			start = pos + delimiterLength;
		}

		return result;
	}

	template <typename TStringView, typename TString>
	inline TString ToLower(TStringView in_str)
	{
		auto result = TString(in_str);

		if (!result.empty())
		{
			std::transform(result.begin(), result.end(), result.begin(),
				[](TString::value_type c) { return std::tolower(c); });
		}

		return result;
	}

	template <typename TStringView, typename TLiteral, typename TString>
	inline TStringView TrimStart(TStringView in_str, const TLiteral in_trimChar)
	{
		const auto startIt = std::find_if(in_str.begin(), in_str.end(),
		//
			[&](typename TStringView::value_type c) -> bool
			{
				if (!in_trimChar)
					return !std::isspace(c);

				return c != in_trimChar;
			}
		);

		return in_str.substr(std::distance(in_str.begin(), startIt));
	}

	template <typename TStringView, typename TLiteral, typename TString>
	inline TStringView TrimEnd(TStringView in_str, const TLiteral in_trimChar)
	{
		const auto endIt = std::find_if_not(in_str.rbegin(), in_str.rend(),
		//
			[&](typename TStringView::value_type c) -> bool
			{
				if (!in_trimChar)
					return std::isspace(c);

				return c == in_trimChar;
			}
		)
		.base();

		return in_str.substr(0, std::distance(in_str.begin(), endIt));
	}

	template <typename TStringView, typename TLiteral, typename TString>
	inline TStringView Trim(TStringView in_str, const TLiteral in_trimChar)
	{
		return TStringView(TrimEnd(TrimStart(in_str, in_trimChar), in_trimChar));
	}

	template <typename TStringView, typename TString>
	inline TString Truncate(TStringView in_str, size_t in_maxLength, bool in_truncateEnd, bool in_ellipsis)
	{
		const auto ellipsisStr = CreateInferredString<TString>("...");

		if (in_maxLength <= ellipsisStr.length())
			return TString(ellipsisStr.substr(0, in_maxLength));

		if (in_str.length() <= in_maxLength)
			return TString(in_str);

		TString result{};
		size_t length = in_maxLength;

		if (in_ellipsis)
			length -= ellipsisStr.length();

		if (in_truncateEnd)
		{
			result = TString(in_str.substr(0, length));

			if (in_ellipsis)
				result += ellipsisStr;
		}
		else
		{
			if (in_ellipsis)
				result += ellipsisStr;

			result = TString(in_str.substr(in_str.length() - length, length));
		}

		return result;
	}

	template <typename TString, typename TValue>
	inline TString ToHex(TValue in_value, size_t in_maxLength, bool in_prefix)
	{
		std::basic_stringstream<typename TString::value_type> result{};

		if (in_prefix)
			result << CreateInferredString<TString>("0x");

		result << std::uppercase << std::hex;

		if (in_maxLength)
			result << std::setw(in_maxLength) << std::setfill((typename TString::value_type)'0');

		result << in_value;

		return result.str();
	}

	template <typename TDst, typename TSrc>
	inline bool TryConvert(TSrc&& in_rrStr, TDst& out_rValue)
	{
		using TSrcChar = hedgedev::csl::ut::expr::GetCharType_t<TSrc>;
		using TDstChar = hedgedev::csl::ut::expr::GetCharType_t<TDst>;

		const auto view = std::basic_string_view<TSrcChar>(in_rrStr);
		const auto size = view.size();
		
		if (!size)
		{
			out_rValue = TDst();
			return true;
		}
		
		if constexpr (std::is_same_v<TSrcChar, TDstChar>)
		{
			out_rValue = TDst(view);
			return true;
		}

		if constexpr (std::is_same_v<TDst, std::string>)
		{
			if constexpr (std::is_same_v<TSrcChar, wchar_t>)
			{
				auto upBuffer = std::make_unique<char[]>(size + sizeof(char));
#ifdef WIN32
				char defaultChar = '?';
				size_t chars = WideCharToMultiByte(CP_UTF8, 0, view.data(), size, (LPSTR)upBuffer.get(), size, &defaultChar, NULL);
				if (chars < size)
					return false;
#else
				size_t chars{};
				if (wcstombs_s(&chars, (char*)upBuffer.get(), size + sizeof(char), view.data(), size) != 0)
					return false;
#endif
				out_rValue = std::string((char*)upBuffer.get(), size);
			}
		}
		else if constexpr (std::is_same_v<TDst, std::wstring>)
		{
			if constexpr (std::is_same_v<TSrcChar, char>)
			{
				auto upBuffer = std::make_unique<wchar_t[]>(size + sizeof(wchar_t));
#ifdef WIN32
				size_t chars = MultiByteToWideChar(CP_UTF8, 0, view.data(), size, (LPWSTR)upBuffer.get(), size);
				if (chars < size)
					return false;
#else
				size_t chars{};
				if (mbstowcs_s(&chars, (wchar_t*)upBuffer.get(), size + sizeof(wchar_t), view.data(), size) != 0)
					return false;
#endif
				out_rValue = std::wstring((wchar_t*)upBuffer.get(), size);
			}
		}
		else
		{
			static_assert(false, "Unsupported destination string type.");
		}

		return true;
	}

	template <typename TResult, typename TStringView>
	inline bool TryParse(TStringView in_str, TResult& out_rValue)
	{
		if constexpr (std::is_same_v<TResult, bool>)
		{
			out_rValue = in_str.at(0) != (typename TStringView::value_type)'0' ||
					ToLower(in_str) == CreateInferredString<TStringView>("true");

			return true;
		}
		else if constexpr (std::is_same_v<TResult, float> || std::is_same_v<TResult, double>)
		{
			TResult result{};
			std::from_chars(in_str.data(), in_str.data() + in_str.size(), result);

			out_rValue = result;

			return true;
		}
		else if constexpr (std::is_integral_v<TResult> || std::is_enum_v<TResult>)
		{
			out_rValue = static_cast<TResult>(std::atoll(in_str.data()));
			return true;
		}

		return false;
	}

	template <typename TStringView, typename TString>
	inline TString Wrap(TStringView in_str, size_t in_maxLength)
	{
		using TStringChar = typename TStringView::value_type;

		constexpr auto lineBreak = TStringChar('\n');
		constexpr auto carriageReturn = TStringChar('\r');

		constexpr TStringChar wordChars[] =
		{
			'.', ',', ';', '!', '?', '\"', '\\', '/'
		};

		TString result{};

		const TStringChar* pos = in_str.data();
		const TStringChar* end = in_str.data() + in_str.size();

		const TStringChar* lastLineStart = pos;
		const TStringChar* lastWordStart{};

		bool isInsideWord = true;

		while (pos < end)
		{
			auto c = *pos;
			auto nextPos = pos + 1;

			if (std::iscntrl(c))
			{
				if (c == TStringChar('\n'))
				{
					result.append(lastLineStart, nextPos);

					pos = nextPos;

					lastLineStart = nextPos;
					lastWordStart = nullptr;

					isInsideWord = true;

					continue;
				}

				if (c == TStringChar('\r'))
				{
					pos = nextPos;
					continue;
				}
			}

			isInsideWord = !std::isspace(c) &&
				std::find(std::begin(wordChars), std::end(wordChars), c) != std::end(wordChars);

			if (!isInsideWord)
				lastWordStart = pos;

			if (size_t(pos - lastLineStart) >= in_maxLength)
			{
				if (lastWordStart && lastWordStart > lastLineStart)
				{
					// Wrap to last word boundary.
					result.append(lastLineStart, lastWordStart);
					result.push_back(TStringChar('\n'));

					lastLineStart = lastWordStart;
					lastWordStart = nullptr;
				}
				else
				{
					// Wrap immediately.
					result.append(lastLineStart, pos);
					result.push_back(TStringChar('\n'));

					lastLineStart = pos;
				}
			}

			pos = nextPos;
		}

		// Handle remaining characters.
		if (lastLineStart < end)
			result.append(lastLineStart, end);

		return result;
	}
}
