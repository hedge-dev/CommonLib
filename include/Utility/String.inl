#include <algorithm>
#include <charconv>
#include <iomanip>
#include <regex>

namespace hedgedev::csl::ut::string
{
    template <expr::AnyString TLeft, expr::AnyString TRight>
    inline bool Compare(const TLeft& in_rLeft, const TRight& in_rRight, bool in_isCaseSensitive)
	{
		const auto strings = PrecedentConvert(in_rLeft, in_rRight);

		const auto& left = strings[0];
		const auto& right = strings[1];
		
		if (left.length() != right.length())
			return false;
		
		if (!in_isCaseSensitive)
			return ToLower(left) == ToLower(right);
		
		return left == right;
	}

    template <expr::AnyString TString, expr::AnyString TSubString>
    inline bool Contains(const TString& in_rStr, const TSubString& in_rSubStr, bool in_isCaseSensitive)
	{
		using TStringView = expr::PrecedentInferredStringView<TString, TSubString>;

		const auto strings = PrecedentConvert(in_rStr, in_rSubStr);

		const auto& rStr = strings[0];
		const auto& rSubStr = strings[1];
		
		if (rStr.length() < rSubStr.length())
			return false;
		
		if (!in_isCaseSensitive)
		{
			using TChar = expr::GetCharType_t<TStringView>;
		
			const auto it = std::search(rStr.begin(), rStr.end(), rSubStr.begin(), rSubStr.end(),
			//
				[](TChar l, TChar r)
				{
					return std::tolower(l) == std::tolower(r);
				}
			);
		
			return it != rStr.end();
		}
		
		return rStr.find(rSubStr) != TStringView::npos;
	}

	template <expr::AnyString TDst, expr::AnyString TSrc>
	inline std::conditional_t<std::is_same_v<TDst, TSrc> && !std::is_pointer_v<TSrc>, const TDst&, TDst> Convert(const TSrc& in_rStr)
	{
		if constexpr (std::is_same_v<TDst, TSrc>)
		{
			return in_rStr;
		}
		else
		{
			TDst result{};

			TryConvert<TDst>(in_rStr, result);

			return result;
		}
	}

	template <expr::AnyString TString, typename TChar>
	inline expr::InferredString<TString> Escape(const TString& in_rStr, TChar in_searchChar, TChar in_escapeChar)
	{
		std::basic_stringstream<TChar> result{};

		for (const auto& c : expr::InferredStringView(in_rStr))
		{
			if (c == in_searchChar)
				result << in_escapeChar;

			result << c;
		}

		return result.str();
	}

	template <expr::AnyString TString>
	inline expr::InferredString<TString> Format(const TString in_str, ...)
	{
		va_list args;
		va_start(args, in_str);

		using TChar = expr::GetCharType_t<TString>;

		const auto view = expr::InferredStringView<TString>(in_str);
		size_t length = sizeof(TChar);

		if constexpr (std::is_same_v<TChar, char>)
		{
			length += _vscprintf(view.data(), args);
		}
		else if constexpr (std::is_same_v<TChar, wchar_t>)
		{
			length += _vscwprintf(view.data(), args);
		}
		else
		{
			static_assert(false, "Unsupported string type.");
		}

		const auto upBuffer = std::make_unique<TChar[]>(length);

		if (length > 0)
		{
			if constexpr (std::is_same_v<TChar, char>)
			{
				_vsnprintf_s(upBuffer.get(), length, _TRUNCATE, view.data(), args);
			}
			else if constexpr (std::is_same_v<TChar, wchar_t>)
			{
				_vsnwprintf_s(upBuffer.get(), length, _TRUNCATE, view.data(), args);
			}
		}

		va_end(args);

		return expr::InferredString<TString>(upBuffer.get(), length);
	}

	template <expr::AnyString TString>
	inline size_t GetWidth(const TString& in_rStr)
	{
		size_t result{};

		std::basic_stringstream<expr::GetCharType_t<TString>> stream(in_rStr);
		expr::InferredString<TString> line{};

		while (std::getline(stream, line))
			result = std::max(result, line.length());

		return result;
	}

	template <expr::AnyString TString, expr::AnyString TUrl, typename TOut>
	inline TOut Hyperlink(const TString& in_rStr, const TUrl& in_rUrl)
	{
		const auto strings = PrecedentConvert(in_rStr, in_rUrl);

		const auto& rStr = strings[0];
		const auto& rUrl = strings[1];

		if (rUrl.empty())
			return TOut(rStr);
		
		std::basic_stringstream<expr::GetCharType_t<TOut>> result{};
		
		result << expr::CreateInferredString<TOut>("<a href=\"")
			   << rUrl << expr::CreateInferredString<TOut>("\">")
			   << rStr << expr::CreateInferredString<TOut>("</a>");
		
		return result.str();
	}

	template <expr::AnyString TDelimiter, expr::AnyString TCollection>
	inline expr::InferredString<TCollection> Join(const TDelimiter& in_rDelimiter, const std::vector<TCollection>& in_rStrings)
	{
		if constexpr (!expr::IsSameUnderlyingCharType<TDelimiter, TCollection>)
			static_assert(false, "The delimiter must have the same underlying character type as the collection.");

		std::basic_stringstream<expr::GetCharType_t<TCollection>> result{};

		if (!in_rStrings.empty())
		{
			const auto length = in_rStrings.size();

			for (size_t i = 0; i < length; i++)
			{
				const auto& rStr = in_rStrings[i];

				result << rStr;

				if (i == length - 1)
					continue;

				result << in_rDelimiter;
			}
		}

		return result.str();
	}

	template <expr::AnyString TDelimiter, expr::AnyString... TArgs, typename TOut>
	inline TOut Join(const TDelimiter& in_rDelimiter, const TArgs&... in_rArgs)
	{
		const auto strings = PrecedentConvert(in_rDelimiter, in_rArgs...);

		return Join(strings[0], std::vector(strings.begin() + 1, strings.end()));
	}

	template <expr::AnyString TString, expr::AnyString TPadString, typename TOut>
	inline TOut Pad(const TString& in_rStr, const TPadString& in_rPadStr)
	{
		std::basic_stringstream<expr::GetCharType_t<TOut>> result{};

		const auto strings = PrecedentConvert(in_rStr, in_rPadStr);

		const auto& rStr = strings[0];
		const auto& rPadStr = strings[1];

		result << rPadStr << rStr << rPadStr;

		return result.str();
	}

	template <typename TOut, expr::AnyString TString>
	inline TOut Parse(const TString& in_rStr)
	{
		TOut result{};

		TryParse(in_rStr, result);

		return result;
	}

	template <expr::AnyString... TArgs, typename TOut>
	inline std::array<TOut, sizeof...(TArgs)> PrecedentConvert(const TArgs&... in_rArgs)
	{
		std::array<TOut, sizeof...(TArgs)> result{};

		const auto args = std::make_tuple(in_rArgs...);
		constexpr auto precedence = expr::GetStringTypePrecedence<TArgs...>();

		[&] <size_t... Index>(std::index_sequence<Index...>)
		{
			const auto convert = [&] <expr::AnyString T>(const size_t in_index, const T& in_rStr)
			{
				if constexpr (expr::IsAllSame<TArgs...>)
				{
					if constexpr (expr::CString<T>)
					{
						// Create inferred string from C string.
						result[in_index] = TOut(in_rStr);
					}
					else
					{
						// Copy original string.
						result[in_index] = in_rStr;
					}
				}
				else
				{
					if constexpr (std::is_same_v<T, TOut>)
					{
						// Copy original string.
						result[in_index] = in_rStr;
					}
					else
					{
						// Convert string to precedent type.
						result[in_index] = Convert<TOut>(in_rStr);
					}
				}
			};

			(convert(Index, std::get<Index>(args)), ...);
		}
		(std::make_index_sequence<sizeof...(TArgs)>{});

		return result;
	}

	template <expr::AnyString TString>
	inline expr::InferredString<TString> RemoveXmlTags(const TString& in_rStr)
	{
		return std::regex_replace(expr::InferredStringView<TString>(in_rStr).data(),
			std::basic_regex<expr::GetCharType_t<TString>>(expr::CreateInferredString<expr::InferredString<TString>>("<[^>]*>")),
			expr::InferredString<TString>());
	}

	template <expr::AnyString TString, expr::AnyString TDelimiter, typename TOut>
	inline std::vector<TOut> Split(const TString& in_rStr, const TDelimiter& in_rDelimiter)
	{
		std::vector<TOut> result{};

		using TStringView = expr::PrecedentInferredStringView<TString, TDelimiter>;

		const auto strings = PrecedentConvert(in_rStr, in_rDelimiter);

		const auto& rStr = strings[0];
		const auto& rDelimiter = strings[1];

		size_t start{};
		const auto delimiterLength = rDelimiter.length();

		while (true)
		{
			const auto pos = rStr.find(rDelimiter, start);
			const auto token = rStr.substr(start, pos == TStringView::npos ? rStr.size() - start : pos - start);

			result.emplace_back(Trim(token));

			if (pos == TStringView::npos)
				break;

			start = pos + delimiterLength;
		}

		return result;
	}

	template <expr::AnyString TString>
	inline expr::InferredString<TString> ToLower(const TString& in_rStr)
	{
		auto result = expr::InferredString<TString>(in_rStr);

		if (!result.empty())
		{
			std::transform(result.begin(), result.end(), result.begin(),
				[](expr::GetCharType_t<TString> c) { return std::tolower(c); });
		}

		return result;
	}

	template <expr::AnyString TString>
	inline expr::InferredString<TString> ToUpper(const TString& in_rStr)
	{
		auto result = expr::InferredString<TString>(in_rStr);

		if (!result.empty())
		{
			std::transform(result.begin(), result.end(), result.begin(),
				[](expr::GetCharType_t<TString> c) { return std::toupper(c); });
		}

		return result;
	}

	template <expr::AnyString TString, typename TChar>
	inline expr::InferredString<TString> TrimStart(const TString& in_rStr, std::optional<std::vector<TChar>> in_trimChars)
	{
		const auto str = expr::InferredStringView<TString>(in_rStr);

		const auto it = std::find_if(str.begin(), str.end(),
		//
			[&](TChar c) -> bool
			{
				if (in_trimChars.has_value())
					return std::find(in_trimChars->begin(), in_trimChars->end(), c) == in_trimChars->end();

				return !std::isspace(c);
			}
		);

		return expr::InferredString<TString>(str.substr(std::distance(str.begin(), it)));
	}

	template <expr::AnyString TString, typename TChar>
	inline expr::InferredString<TString> TrimEnd(const TString& in_rStr, std::optional<std::vector<TChar>> in_trimChars)
	{
		const auto str = expr::InferredStringView<TString>(in_rStr);

		const auto it = std::find_if_not(str.rbegin(), str.rend(),
		//
			[&](TChar c) -> bool
			{
				if (in_trimChars.has_value())
					return std::find(in_trimChars->begin(), in_trimChars->end(), c) != in_trimChars->end();

				return std::isspace(c);
			}
		)
		.base();

		return expr::InferredString<TString>(str.substr(0, std::distance(str.begin(), it)));
	}

	template <expr::AnyString TString, typename TChar>
	inline expr::InferredString<TString> Trim(const TString& in_rStr, const std::optional<std::vector<TChar>> in_trimChars)
	{
		return TrimEnd(TrimStart(in_rStr, in_trimChars), in_trimChars);
	}

	template <expr::AnyString TString>
	inline expr::InferredString<TString> Truncate(const TString& in_rStr, size_t in_maxLength, bool in_truncateEnd, bool in_ellipsis)
	{
		using TOut = expr::InferredString<TString>;

		const auto ellipsis = expr::CreateInferredString<TOut>("...");

		// Truncate ellipsis to max length.
		if (in_ellipsis && in_maxLength <= ellipsis.length())
			return TOut(ellipsis.substr(0, in_maxLength));

		const auto str = expr::InferredStringView<TString>(in_rStr);

		if (str.length() <= in_maxLength)
			return TOut(str);

		TOut result{};
		auto length = in_maxLength;

		if (in_ellipsis)
			length -= ellipsis.length();

		if (in_truncateEnd)
		{
			result = TOut(str.substr(0, length));

			if (in_ellipsis)
				result += ellipsis;
		}
		else
		{
			if (in_ellipsis)
				result += ellipsis;

			result = TOut(str.substr(str.length() - length, length));
		}

		return result;
	}

	template <expr::BasicString TString, typename TValue>
	inline TString ToHex(TValue in_value, size_t in_maxLength, bool in_prefix)
	{
		using TChar = expr::GetCharType_t<TString>;

		std::basic_stringstream<TChar> result{};

		if (in_prefix)
			result << expr::CreateInferredString<TString>("0x");

		result << std::uppercase << std::hex;

		if (in_maxLength)
			result << std::setw(in_maxLength) << std::setfill(TChar('0'));

		result << in_value;

		return result.str();
	}

	template <expr::AnyString TDst, expr::AnyString TSrc>
	inline bool TryConvert(const TSrc& in_rSrc, TDst& out_rDst)
	{
		using TSrcChar = expr::GetCharType_t<TSrc>;
		using TDstChar = expr::GetCharType_t<TDst>;

		const auto view = expr::InferredStringView<TSrc>(in_rSrc);
		const auto size = view.size();
		
		if (!size)
		{
			out_rDst = TDst();
			return true;
		}

		if constexpr (std::is_same_v<TSrc, TDst>)
		{
			out_rDst = in_rSrc;
			return true;
		}
		else if constexpr (std::is_same_v<TSrcChar, TDstChar>)
		{
			out_rDst = TDst(view);
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
				out_rDst = std::string((char*)upBuffer.get(), size);
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
				out_rDst = std::wstring((wchar_t*)upBuffer.get(), size);
			}
		}
		else
		{
			static_assert(false, "Unsupported destination string type.");
		}

		return true;
	}

	template <typename TOut, expr::AnyString TString>
	inline bool TryParse(const TString& in_rStr, TOut& out_rValue)
	{
		const auto str = Trim(in_rStr);

		if constexpr (std::is_same_v<TOut, bool>)
		{
			out_rValue = ToLower(str) == expr::CreateInferredString<expr::InferredString<TString>>("true") ||
						 str == expr::CreateInferredString<expr::InferredString<TString>>("1");

			return true;
		}
		else if constexpr (std::is_same_v<TOut, float> || std::is_same_v<TOut, double>)
		{
			TOut result{};
			std::from_chars(str.data(), str.data() + str.size(), result);

			out_rValue = result;

			return true;
		}
		else if constexpr (std::is_integral_v<TOut> || std::is_enum_v<TOut>)
		{
			out_rValue = static_cast<TOut>(std::atoll(str.data()));
			return true;
		}

		return false;
	}

	template <expr::AnyString TString>
	inline expr::InferredString<TString> Wrap(const TString& in_rStr, size_t in_maxWidth)
	{
		expr::InferredString<TString> result{};

		const auto str = expr::InferredStringView<TString>(in_rStr);

		using TChar = expr::GetCharType_t<TString>;

		static constexpr auto s_kLineBreak = TChar('\n');
		static constexpr auto s_kCarriageReturn = TChar('\r');

		static constexpr TChar s_kWordChars[] =
		{
			'.', ',', ';', '!', '?', '\"', '\\', '/'
		};

		const TChar* pos = str.data();
		const TChar* end = str.data() + str.size();

		const TChar* lastLineStart = pos;
		const TChar* lastWordStart{};

		bool isInsideWord = true;

		while (pos < end)
		{
			auto c = *pos;
			auto nextPos = pos + 1;

			if (std::iscntrl(c))
			{
				if (c == s_kLineBreak)
				{
					result.append(lastLineStart, nextPos);

					pos = nextPos;

					lastLineStart = nextPos;
					lastWordStart = nullptr;

					isInsideWord = true;

					continue;
				}

				if (c == s_kCarriageReturn)
				{
					pos = nextPos;
					continue;
				}
			}

			isInsideWord = !std::isspace(c) &&
				std::find(std::begin(s_kWordChars), std::end(s_kWordChars), c) != std::end(s_kWordChars);

			if (!isInsideWord)
				lastWordStart = pos;

			if (size_t(pos - lastLineStart) >= in_maxWidth)
			{
				if (lastWordStart && lastWordStart > lastLineStart)
				{
					// Wrap to last word boundary.
					result.append(lastLineStart, lastWordStart);
					result.push_back(s_kLineBreak);

					lastLineStart = lastWordStart;
					lastWordStart = nullptr;
				}
				else
				{
					// Wrap immediately.
					result.append(lastLineStart, pos);
					result.push_back(s_kLineBreak);

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
