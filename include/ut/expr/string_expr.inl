#include <tuple>

namespace hedgedev::csl::ut::expr
{
    template <any_string T_result, any_string T_str>
    inline constexpr T_result create_inferred_string(const T_str& in_str)
    {
        T_result result{};

        const auto str_sv = inferred_string_view_t<T_str>(in_str);

        result.reserve(str_sv.size() * sizeof(get_char_type_t<T_result>));

        std::transform(str_sv.begin(), str_sv.end(), std::back_inserter(result), [](get_char_type_t<T_str> in_char)
        {
            return static_cast<get_char_type_t<T_result>>(in_char);
        });

        return result;
    }

    template <any_string... T_args>
    inline constexpr size_t get_string_type_precedence()
    {
        std::tuple<size_t, size_t> result{ 0, 0 };
        size_t i{};

        (
            [&]
            {
                size_t size = sizeof(get_char_type_t<T_args>);

                if (std::get<1>(result) < size)
                    result = { i, size };

                i++;
            }

            (), ...
        );

        return std::get<0>(result);
    }
}
