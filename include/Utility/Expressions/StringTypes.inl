namespace hedgedev::csl::ut::expr
{
    template <AnyString TDst, AnyString TSrc>
    inline constexpr TDst CreateInferredString(const TSrc& in_rStr)
    {
        const auto result = InferredStringView<TSrc>(in_rStr);

        return TDst(result.begin(), result.end());
    }

    template <AnyString... TArgs>
    inline constexpr size_t GetStringTypePrecedence()
    {
        std::tuple<size_t, size_t> result{ 0, 0 };
        size_t i{};

        (
            [&]
            {
                size_t size = sizeof(GetCharType_t<TArgs>);

                if (std::get<1>(result) < size)
                    result = { i, size };

                i++;
            }

            (), ...
        );

        return std::get<0>(result);
    }
}
