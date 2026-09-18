#include <algorithm>
#include <charconv>
#include <filesystem>
#include <iomanip>
#include <regex>
#include <type_traits>

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#endif

#include "expr/expr.h"
#include "expr/string_types.h"

namespace hedgedev::csl::ut::string
{
    template <expr::any_string_t T_left, expr::any_string_t T_right>
    inline bool compare(const T_left& in_left, const T_right& in_right, bool in_case_sensitive)
	{
		const auto strings = precedent_convert(in_left, in_right);

		const auto& left = strings[0];
		const auto& right = strings[1];
		
		if (left.length() != right.length())
			return false;
		
		if (!in_case_sensitive)
			return lower(left) == lower(right);
		
		return left == right;
	}

    template <expr::any_string_t T_str, expr::any_string_t T_substr>
    inline bool contains(const T_str& in_str, const T_substr& in_substr, bool in_case_sensitive)
	{
		const auto strings = precedent_convert(in_str, in_substr);

		const auto& str = strings[0];
		const auto& substr = strings[1];
		
		if (str.length() < substr.length())
			return false;

		using pi_string_t = expr::pi_string_t<T_str, T_substr>;
		
		if (!in_case_sensitive)
		{
			using pi_string_char_t = expr::get_char_type_t<pi_string_t>;
		
			const auto it = std::search(str.begin(), str.end(), substr.begin(), substr.end(),
			//
				[](pi_string_char_t in_left, pi_string_char_t in_right) -> bool
				{
					return std::tolower(in_left) == std::tolower(in_right);
				}
			);
		
			return it != str.end();
		}
		
		return str.find(substr) != pi_string_t::npos;
	}

	template <expr::any_string_t T_result, expr::any_string_t T_str>
	inline std::conditional_t<std::is_same_v<T_result, T_str> && !std::is_pointer_v<T_str>, const T_result&, T_result> convert(const T_str& in_str)
	{
		if constexpr (std::is_same_v<T_result, T_str>)
		{
			return in_str;
		}
		else
		{
			T_result result{};

			try_convert<T_result>(in_str, result);

			return result;
		}
	}

	template <expr::any_string_t T_str, typename T_str_char>
	inline expr::inferred_string_t<T_str> escape(const T_str& in_str, T_str_char in_search_char, T_str_char in_escape_char)
	{
		std::basic_stringstream<T_str_char> result{};

		for (const auto& c : expr::inferred_string_view_t(in_str))
		{
			if (c == in_search_char)
				result << in_escape_char;

			result << c;
		}

		return result.str();
	}

	template <expr::any_string_t T>
	inline expr::inferred_string_t<T> format(const T in_str, ...)
	{
		va_list args;
		va_start(args, in_str);

		using str_char_t = expr::get_char_type_t<T>;

		const auto str_sv = expr::inferred_string_view_t<T>(in_str);

		size_t length = sizeof(str_char_t);

		if constexpr (std::is_same_v<str_char_t, char>)
		{
			length += _vscprintf(str_sv.data(), args);
		}
		else if constexpr (std::is_same_v<str_char_t, wchar_t>)
		{
			length += _vscwprintf(str_sv.data(), args);
		}
		else
		{
			static_assert(false, "Unsupported string type.");
		}

		const auto buffer = std::make_unique<str_char_t[]>(length);

		if (length > 0)
		{
			if constexpr (std::is_same_v<str_char_t, char>)
			{
				_vsnprintf_s(buffer.get(), length, _TRUNCATE, str_sv.data(), args);
			}
			else if constexpr (std::is_same_v<str_char_t, wchar_t>)
			{
				_vsnwprintf_s(buffer.get(), length, _TRUNCATE, str_sv.data(), args);
			}
		}

		va_end(args);

		return expr::inferred_string_t<T>(buffer.get(), length);
	}

	template <expr::any_string_t T>
	inline size_t get_width(const T& in_str)
	{
		size_t result{};

		std::basic_stringstream<expr::get_char_type_t<T>> stream(in_str);
		expr::inferred_string_t<T> line{};

		while (std::getline(stream, line))
			result = std::max(result, line.length());

		return result;
	}

	template <expr::any_string_t T_str, expr::any_string_t T_url, typename T_result>
	inline T_result hyperlink(const T_str& in_str, const T_url& in_url)
	{
		const auto strings = precedent_convert(in_str, in_url);

		const auto& str = strings[0];
		const auto& url = strings[1];

		if (url.empty())
			return T_result(str);
		
		std::basic_stringstream<expr::get_char_type_t<T_result>> result{};
		
		result << expr::create_inferred_string<T_result>("<a href=\"")
			   << url << expr::create_inferred_string<T_result>("\">")
			   << str << expr::create_inferred_string<T_result>("</a>");
		
		return result.str();
	}

	template <expr::any_string_t T>
	inline bool is_null_or_empty(const T& in_str)
	{
		if constexpr (std::is_pointer_v<std::remove_cvref_t<T>>)
		{
			if (!in_str)
				return true;
		}

		return expr::inferred_string_view_t<T>(in_str).empty();
	}

	template <expr::any_string_t T>
	inline bool is_null_or_whitespace(const T& in_str)
	{
		if (is_null_or_empty(in_str))
			return true;

		const auto str_sv = expr::inferred_string_view_t<T>(in_str);

		return std::all_of(str_sv.begin(), str_sv.end(), [](expr::get_char_type_t<T> in_char) -> bool
		{
			return std::isspace(in_char);
		});
	}

	template <expr::any_string_t T_delimiter, expr::any_string_t T_strings>
	inline expr::inferred_string_t<T_strings> join(const T_delimiter& in_delimiter, const std::vector<T_strings>& in_strings)
	{
		if constexpr (!expr::is_same_underlying_char_type_v<T_delimiter, T_strings>)
			static_assert(false, "The delimiter must have the same underlying character type as the collection.");

		std::basic_stringstream<expr::get_char_type_t<T_strings>> result{};

		if (!in_strings.empty())
		{
			const auto length = in_strings.size();

			for (size_t i = 0; i < length; i++)
			{
				const auto& str = in_strings[i];

				result << str;

				if (i == length - 1)
					continue;

				result << in_delimiter;
			}
		}

		return result.str();
	}

	template <expr::any_string_t T_delimiter, expr::any_string_t... T_args, typename T_result>
	inline T_result join(const T_delimiter& in_delimiter, const T_args&... in_args)
	{
		const auto strings = precedent_convert(in_delimiter, in_args...);

		return join(strings[0], std::vector(strings.begin() + 1, strings.end()));
	}

	template <expr::any_string_t T_str, expr::any_string_t T_pad_str, typename T_result>
	inline T_result pad(const T_str& in_str, const T_pad_str& in_pad_str)
	{
		std::basic_stringstream<expr::get_char_type_t<T_result>> result{};

		const auto strings = precedent_convert(in_str, in_pad_str);

		const auto& str = strings[0];
		const auto& pad_str = strings[1];

		result << pad_str << str << pad_str;

		return result.str();
	}

	template <typename T_result, expr::any_string_t T_str>
	inline T_result parse(const T_str& in_str)
	{
		T_result result{};

		try_parse(in_str, result);

		return result;
	}

	template <expr::any_string_t... T_args, typename T_result>
	inline std::array<T_result, sizeof...(T_args)> precedent_convert(const T_args&... in_args)
	{
		std::array<T_result, sizeof...(T_args)> result{};

		const auto args = std::make_tuple(in_args...);
		constexpr auto precedence = expr::get_string_type_precedence<T_args...>();

		[&] <size_t... K_index>(std::index_sequence<K_index...>)
		{
			const auto convert_arg = [&] <expr::any_string_t T>(const size_t in_index, const T& in_str)
			{
				if constexpr (expr::is_all_same_v<T_args...>)
				{
					if constexpr (expr::raw_string_t<T>)
					{
						// Create inferred string from C string.
						result[in_index] = T_result(in_str);
					}
					else
					{
						// Copy original string.
						result[in_index] = in_str;
					}
				}
				else
				{
					if constexpr (std::is_same_v<T, T_result>)
					{
						// Copy original string.
						result[in_index] = in_str;
					}
					else
					{
						// Convert string to precedent type.
						result[in_index] = convert<T_result>(in_str);
					}
				}
			};

			(convert_arg(K_index, std::get<K_index>(args)), ...);
		}
		(std::make_index_sequence<sizeof...(T_args)>{});

		return result;
	}

	template <expr::any_string_t T_str>
	inline expr::inferred_string_t<T_str> remove_xml_tags(const T_str& in_str)
	{
		const auto xml_regex = expr::create_inferred_string<expr::inferred_string_t<T_str>>("<[^>]*>");

		return std::regex_replace(expr::inferred_string_view_t<T_str>(in_str).data(),
			std::basic_regex<expr::get_char_type_t<T_str>>(xml_regex),
			expr::inferred_string_t<T_str>());
	}

	template <expr::any_string_t T_str, expr::any_string_t T_delimiter, typename T_result>
	inline std::vector<T_result> split(const T_str& in_str, const T_delimiter& in_delimiter)
	{
		std::vector<T_result> result{};

		using pi_string_view_t = expr::pi_string_view_t<T_str, T_delimiter>;

		const auto strings = precedent_convert(in_str, in_delimiter);

		const auto& str = strings[0];
		const auto& delimiter = strings[1];

		size_t start{};
		const auto delimiter_length = delimiter.length();

		while (true)
		{
			const auto pos = str.find(delimiter, start);
			const auto token = str.substr(start, pos == pi_string_view_t::npos ? str.size() - start : pos - start);

			result.emplace_back(trim(token));

			if (pos == pi_string_view_t::npos)
				break;

			start = pos + delimiter_length;
		}

		return result;
	}

	template <expr::any_string_t T>
	inline expr::inferred_string_t<T> lower(const T& in_str)
	{
		auto result = expr::inferred_string_t<T>(in_str);

		if (!result.empty())
		{
			std::transform(result.begin(), result.end(), result.begin(),
			//
				[](expr::get_char_type_t<T> in_char) -> bool
				{
					return std::tolower(in_char);
				}
			);
		}

		return result;
	}

	template <expr::any_string_t T>
	inline expr::inferred_string_t<T> upper(const T& in_str)
	{
		auto result = expr::inferred_string_t<T>(in_str);

		if (!result.empty())
		{
			std::transform(result.begin(), result.end(), result.begin(),
			//
				[](expr::get_char_type_t<T> in_char) -> bool
				{
					return std::toupper(in_char);
				}
			);
		}

		return result;
	}

	template <expr::any_string_t T_str, typename T_trim_chars>
	inline expr::inferred_string_t<T_str> trim_start(const T_str& in_str, std::optional<std::vector<T_trim_chars>> in_trim_chars)
	{
		const auto str_sv = expr::inferred_string_view_t<T_str>(in_str);

		const auto it = std::find_if(str_sv.begin(), str_sv.end(),
		//
			[&](T_trim_chars in_char) -> bool
			{
				if (in_trim_chars.has_value())
					return std::find(in_trim_chars->begin(), in_trim_chars->end(), in_char) == in_trim_chars->end();

				return !std::isspace(in_char);
			}
		);

		return expr::inferred_string_t<T_str>(str_sv.substr(std::distance(str_sv.begin(), it)));
	}

	template <expr::any_string_t T_str, typename T_trim_chars>
	inline expr::inferred_string_t<T_str> trim_end(const T_str& in_str, std::optional<std::vector<T_trim_chars>> in_trim_chars)
	{
		const auto str_sv = expr::inferred_string_view_t<T_str>(in_str);

		const auto it = std::find_if_not(str_sv.rbegin(), str_sv.rend(),
		//
			[&](T_trim_chars in_char) -> bool
			{
				if (in_trim_chars.has_value())
					return std::find(in_trim_chars->begin(), in_trim_chars->end(), in_char) != in_trim_chars->end();

				return std::isspace(in_char);
			}
		)
		.base();

		return expr::inferred_string_t<T_str>(str_sv.substr(0, std::distance(str_sv.begin(), it)));
	}

	template <expr::any_string_t T_str, typename T_trim_chars>
	inline expr::inferred_string_t<T_str> trim(const T_str& in_str, const std::optional<std::vector<T_trim_chars>> in_trim_chars)
	{
		return trim_end(trim_start(in_str, in_trim_chars), in_trim_chars);
	}

	template <expr::any_string_t T>
	inline expr::inferred_string_t<T> truncate(const T& in_str, size_t in_max_length, bool in_truncate_end, bool in_ellipsis)
	{
		using inferred_string_t = expr::inferred_string_t<T>;

		inferred_string_t result{};

		const auto ellipsis = expr::create_inferred_string<inferred_string_t>("...");

		if (in_ellipsis && in_max_length <= ellipsis.length())
		{
			// Truncate ellipsis to max length.
			result = ellipsis.substr(0, in_max_length);
		}
		else
		{
			const auto str_sv = expr::inferred_string_view_t<T>(in_str);

			if (str_sv.length() <= in_max_length)
			{
				result = str_sv;
			}
			else
			{
				auto length = in_max_length;

				if (in_ellipsis)
					length -= ellipsis.length();

				if (in_truncate_end)
				{
					result = inferred_string_t(str_sv.substr(0, length));

					if (in_ellipsis)
						result += ellipsis;
				}
				else
				{
					if (in_ellipsis)
						result = ellipsis;

					result += inferred_string_t(str_sv.substr(str_sv.length() - length, length));
				}
			}
		}

		return result;
	}

	template <expr::basic_string_t T_result, typename T_value>
	inline T_result hex(T_value in_value, size_t in_max_length, bool in_prefix)
	{
		using string_char_t = expr::get_char_type_t<T_result>;

		std::basic_stringstream<string_char_t> result{};

		if (in_prefix)
			result << expr::create_inferred_string<T_result>("0x");

		result << std::uppercase << std::hex;

		if (in_max_length)
			result << std::setw(in_max_length) << std::setfill(string_char_t('0'));

		result << in_value;

		return result.str();
	}

	template <expr::any_string_t T_result, expr::any_string_t T_str>
	inline bool try_convert(const T_str& in_str, T_result& out_result)
	{
		using str_char_t = expr::get_char_type_t<T_str>;
		using result_char_t = expr::get_char_type_t<T_result>;

		const auto str_sv = expr::inferred_string_view_t<T_str>(in_str);
		const auto str_size = str_sv.size();
		
		if (!str_size)
		{
			out_result = T_result();
			return true;
		}

		if constexpr (std::is_same_v<T_str, T_result>)
		{
			out_result = in_str;
			return true;
		}
		else if constexpr (std::is_same_v<str_char_t, result_char_t>)
		{
			out_result = T_result(str_sv);
			return true;
		}

		if constexpr (std::is_same_v<T_result, std::string>)
		{
			if constexpr (std::is_same_v<str_char_t, wchar_t>)
			{
				auto buffer = std::make_unique<char[]>(str_size + sizeof(char));
#ifdef WIN32
				size_t chars = WideCharToMultiByte(CP_UTF8, 0, str_sv.data(), int(str_size), (LPSTR)buffer.get(), int(str_size), NULL, NULL);
				if (chars < str_size)
					return false;
#else
				size_t chars{};
				if (wcstombs_s(&chars, (char*)buffer.get(), str_size + sizeof(char), str_sv.data(), str_size) != 0)
					return false;
#endif
				out_result = std::string((char*)buffer.get(), str_size);
			}
		}
		else if constexpr (std::is_same_v<T_result, std::wstring>)
		{
			if constexpr (std::is_same_v<str_char_t, char>)
			{
				auto buffer = std::make_unique<wchar_t[]>(str_size + sizeof(wchar_t));
#ifdef WIN32
				size_t chars = MultiByteToWideChar(CP_UTF8, 0, str_sv.data(), int(str_size), (LPWSTR)buffer.get(), int(str_size));
				if (chars < str_size)
					return false;
#else
				size_t chars{};
				if (mbstowcs_s(&chars, (wchar_t*)buffer.get(), str_size + sizeof(wchar_t), str_sv.data(), str_size) != 0)
					return false;
#endif
				out_result = std::wstring((wchar_t*)buffer.get(), str_size);
			}
		}
		else
		{
			static_assert(false, "Unsupported destination string type.");
		}

		return true;
	}

	template <typename T_result, expr::any_string_t T_str>
	inline bool try_parse(const T_str& in_str, T_result& out_result)
	{
		if (is_null_or_whitespace(in_str))
			return false;

		if constexpr (std::is_same_v<T_str, T_result>)
		{
			out_result = in_str;
			return true;
		}
		else if constexpr (std::is_same_v<T_result, std::filesystem::path>)
		{
			out_result = std::filesystem::path(in_str);
			return true;
		}
		else if constexpr (expr::any_string_t<T_result>)
		{
			T_result result{};

			if (!try_convert<T_result>(in_str, result))
				return false;

			out_result = result;

			return true;
		}

		const auto str_trim = trim(in_str);

		if constexpr (std::is_same_v<T_result, bool>)
		{
			const auto str_trim_lower = lower(str_trim);

			out_result = str_trim_lower == expr::create_inferred_string<expr::inferred_string_t<T_str>>("true") ||
						 str_trim_lower == expr::create_inferred_string<expr::inferred_string_t<T_str>>("1");

			return true;
		}
		else if constexpr (std::is_integral_v<T_result> || std::is_enum_v<T_result> || std::is_same_v<T_result, float> || std::is_same_v<T_result, double>)
		{
			typename std::conditional_t<std::is_enum_v<T_result>, std::underlying_type<T_result>, std::type_identity<T_result>>::type result{};
			std::string chars{};

			if (!try_convert<std::string>(str_trim, chars))
				return false;

			if (std::from_chars(chars.data(), chars.data() + chars.size(), result).ec != std::errc{})
				return false;

			out_result = T_result(result);

			return true;
		}

		return false;
	}

	template <expr::any_string_t T_str>
	inline expr::inferred_string_t<T_str> wrap(const T_str& in_str, size_t in_max_width)
	{
		expr::inferred_string_t<T_str> result{};

		const auto str_sv = expr::inferred_string_view_t<T_str>(in_str);

		using string_char_t = expr::get_char_type_t<T_str>;

		static constexpr auto k_line_break = string_char_t('\n');
		static constexpr auto k_carriage_return = string_char_t('\r');

		static constexpr string_char_t k_word_chars[] =
		{
			'.', ',', ';', '!', '?', '\"', '\\', '/'
		};

		const string_char_t* pos = str_sv.data();
		const string_char_t* end = str_sv.data() + str_sv.size();

		const string_char_t* last_line_start = pos;
		const string_char_t* last_word_start{};

		bool is_inside_word = true;

		while (pos < end)
		{
			auto c = *pos;
			auto next_pos = pos + 1;

			if (std::iscntrl(c))
			{
				if (c == k_line_break)
				{
					result.append(last_line_start, next_pos);

					pos = next_pos;

					last_line_start = next_pos;
					last_word_start = nullptr;

					is_inside_word = true;

					continue;
				}

				if (c == k_carriage_return)
				{
					pos = next_pos;
					continue;
				}
			}

			is_inside_word = !std::isspace(c) &&
				std::find(std::begin(k_word_chars), std::end(k_word_chars), c) != std::end(k_word_chars);

			if (!is_inside_word)
				last_word_start = pos;

			if (size_t(pos - last_line_start) >= in_max_width)
			{
				if (last_word_start && last_word_start > last_line_start)
				{
					// Wrap to last word boundary.
					result.append(last_line_start, last_word_start);
					result.push_back(k_line_break);

					last_line_start = last_word_start;
					last_word_start = nullptr;
				}
				else
				{
					// Wrap immediately.
					result.append(last_line_start, pos);
					result.push_back(k_line_break);

					last_line_start = pos;
				}
			}

			pos = next_pos;
		}

		// Handle remaining characters.
		if (last_line_start < end)
			result.append(last_line_start, end);

		return result;
	}
}
