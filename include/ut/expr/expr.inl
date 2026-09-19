#include <type_traits>

namespace hedgedev::csl::ut::expr
{
    template <class T, typename>
    inline constexpr bool has_flag(T in_mask, T in_flag)
    {
        using mask_t = std::underlying_type_t<T>;

        return (static_cast<mask_t>(in_mask) & static_cast<mask_t>(in_flag)) == static_cast<mask_t>(in_flag);
    }
}
