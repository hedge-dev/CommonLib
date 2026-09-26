#include <type_traits>

#include "../thirdparty/simdutf/singleheader/simdutf.h"
#include "expr/encoding_expr.h"
#include "expr/string_expr.h"
#include "mem/mem.h"
#include "preprocessor.h"

#define __CMNLIB_INTERNAL_STATIC_LIB_ENROLMENT

namespace hedgedev::csl::ut::encoding
{
	template <expr::basic_string T_result, expr::any_string T_str>
	inline std::conditional_t<std::is_same_v<T_result, T_str> && !std::is_pointer_v<T_str>, const T_result&, T_result> convert(const T_str& in_str, encoding_type in_encoding)
	{
		if constexpr (std::is_same_v<T_result, T_str>)
		{
			return in_str;
		}
		else
		{
			T_result result{};

			try_convert<T_result, T_str>(in_str, result, in_encoding);

			return result;
		}
	}

	template <encoding_type encoding, expr::any_string T_str, typename T_result>
	inline std::conditional_t<std::is_same_v<T_result, T_str> && !std::is_pointer_v<T_str>, const T_result&, T_result> convert(const T_str& in_str)
	{
		if constexpr (std::is_same_v<T_result, T_str>)
		{
			return in_str;
		}
		else
		{
			T_result result{};

			try_convert<encoding, T_str, T_result>(in_str, result);

			return result;
		}
	}

	template <expr::any_string... T_args, typename T_result>
	inline std::array<T_result, sizeof...(T_args)> precedent_convert(const T_args&... in_args)
	{
		std::array<T_result, sizeof...(T_args)> result{};

		const auto args = std::make_tuple(in_args...);
		constexpr auto precedence = expr::get_string_type_precedence<T_args...>();

		[&] <size_t... index>(std::index_sequence<index...>)
		{
			const auto convert_arg = [&] <expr::any_string T>(const size_t in_index, const T& in_str)
			{
				if constexpr (expr::is_all_same_v<T_args...>)
				{
					if constexpr (expr::any_raw_string<T>)
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

			(convert_arg(index, std::get<index>(args)), ...);
		}
		(std::make_index_sequence<sizeof...(T_args)>{});

		return result;
	}

	template <expr::basic_string T_result, expr::any_string T_str>
	inline bool try_convert(const T_str& in_str, T_result& out_result, encoding_type in_encoding)
	{
		using src_char_t = expr::get_char_type_t<T_str>;
		using dst_char_t = expr::get_char_type_t<T_result>;

		ASSERT_RETURN_FALSE(sizeof(dst_char_t) == expr::get_encoding_char_size(in_encoding));

		using inferred_string_view_t = expr::inferred_string_view_t<T_str>;

		inferred_string_view_t str_sv{};

		if constexpr (std::is_constructible_v<inferred_string_view_t, const T_str&>)
		{
			str_sv = inferred_string_view_t(in_str);
		}
		else
		{
			// Allow for vector-like types.
			str_sv = inferred_string_view_t(in_str.begin(), in_str.end());
		}

		if constexpr (std::is_same_v<src_char_t, dst_char_t>)
		{
			out_result = T_result(str_sv);
			return true;
		}

		if (str_sv.empty())
		{
			out_result = T_result();
			return true;
		}

		const auto src_data = str_sv.data();
		const auto src_length_in_chars = str_sv.size();
		const auto src_length_in_bytes = src_length_in_chars * sizeof(dst_char_t);

		const auto transcode = [&](auto* out_buffer = nullptr) -> size_t
		{
			if constexpr (sizeof(src_char_t) == 1)
			{
				const auto src = reinterpret_cast<const char*>(src_data);

				if constexpr (sizeof(dst_char_t) == 1)
				{
					return src_length_in_bytes;
				}
				else if constexpr (sizeof(dst_char_t) == 2)
				{
					if (!out_buffer)
						return simdutf::utf16_length_from_utf8(src, src_length_in_chars);

					return simdutf::convert_utf8_to_utf16(src, src_length_in_chars, reinterpret_cast<char16_t*>(out_buffer));
				}
				else if constexpr (sizeof(dst_char_t) == 4)
				{
					if (!out_buffer)
						return simdutf::utf32_length_from_utf8(src, src_length_in_chars);

					return simdutf::convert_utf8_to_utf32(src, src_length_in_chars, reinterpret_cast<char32_t*>(out_buffer));
				}
			}
			else if constexpr (sizeof(src_char_t) == 2)
			{
				const auto src = reinterpret_cast<const char16_t*>(src_data);

				if constexpr (sizeof(dst_char_t) == 1)
				{
					if (!out_buffer)
						return simdutf::utf8_length_from_utf16(src, src_length_in_chars);

					return simdutf::convert_utf16_to_utf8(src, src_length_in_chars, reinterpret_cast<char*>(out_buffer));
				}
				else if constexpr (sizeof(dst_char_t) == 2)
				{
					return src_length_in_bytes;
				}
				else if constexpr (sizeof(dst_char_t) == 4)
				{
					if (!out_buffer)
						return simdutf::utf32_length_from_utf16(src, src_length_in_chars);

					return simdutf::convert_utf16_to_utf32(src, src_length_in_chars, reinterpret_cast<char32_t*>(out_buffer));
				}
			}
			else if constexpr (sizeof(src_char_t) == 4)
			{
				const auto src = reinterpret_cast<const char32_t*>(src_data);

				if constexpr (sizeof(dst_char_t) == 1)
				{
					if (!out_buffer)
						return simdutf::utf8_length_from_utf32(src, src_length_in_chars);

					return simdutf::convert_utf32_to_utf8(src, src_length_in_chars, reinterpret_cast<char*>(out_buffer));
				}
				else if constexpr (sizeof(dst_char_t) == 2)
				{
					if (!out_buffer)
						return simdutf::utf16_length_from_utf32(src, src_length_in_chars);

					return simdutf::convert_utf32_to_utf16(src, src_length_in_chars, reinterpret_cast<char16_t*>(out_buffer));
				}
				else if constexpr (sizeof(dst_char_t) == 4)
				{
					return src_length_in_bytes;
				}
			}

			return 0;
		};

		const auto expected_length = transcode(static_cast<dst_char_t*>(nullptr));

		if (!expected_length)
			return false;

		out_result.resize(expected_length);

		size_t transcode_length{};

		if (expected_length == src_length_in_bytes)
		{
			transcode_length = expected_length;
			out_result = T_result(str_sv.begin(), str_sv.end());
		}
		else
		{
			// Only transcode string if the length does not match.
			// Matching lengths could be an attempt to endian swap instead.
			transcode_length = transcode(out_result.data());
		}

		if (!transcode_length)
		{
			out_result.clear();
			return false;
		}
		else if (transcode_length < expected_length)
		{
			out_result.resize(transcode_length);
		}

		const auto is_big_endian = ut::expr::has_flag(in_encoding, utf16_be) ||
								   ut::expr::has_flag(in_encoding, utf32_be);
		
		const auto is_utf16 = ut::expr::has_flag(in_encoding, utf16_le) ||
							  ut::expr::has_flag(in_encoding, utf16_be);

		const auto is_utf32 = ut::expr::has_flag(in_encoding, utf32_le) ||
							  ut::expr::has_flag(in_encoding, utf32_be);

		auto needs_endian_swap = false;

		if constexpr (std::endian::native == std::endian::little)
		{
			needs_endian_swap = is_big_endian;
		}
		else if constexpr (std::endian::native == std::endian::big)
		{
			needs_endian_swap = !is_big_endian;
		}

		if (needs_endian_swap)
		{
			if (is_utf16)
			{
				auto chars = reinterpret_cast<const char16_t*>(out_result.data());
				const auto chars_length = transcode_length;

				for (size_t i = 0; i < chars_length; i++)
					const_cast<char16_t*>(chars)[i] = mem::byteswap(chars[i]);
			}
			else if (is_utf32)
			{
				auto chars = reinterpret_cast<const char32_t*>(out_result.data());
				const auto chars_length = transcode_length;

				for (size_t i = 0; i < chars_length; i++)
					const_cast<char32_t*>(chars)[i] = mem::byteswap(chars[i]);
			}
		}

		return true;
	}

	template <encoding_type encoding, expr::any_string T_str, typename T_result>
	inline bool try_convert(const T_str& in_str, T_result& out_result)
	{
		return try_convert<T_result, T_str>(in_str, out_result, encoding);
	}
}
