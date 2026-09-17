#include <tuple>

namespace hedgedev::csl::ut::expr
{
    template <any_string_t T_result, any_string_t T_str>
    inline constexpr T_result create_inferred_string(const T_str& in_str)
    {
        const auto result = inferred_string_view_t<T_str>(in_str);

        return T_result(result.begin(), result.end());
    }

    template <any_string_t... T_args>
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
