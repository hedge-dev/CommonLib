#pragma once

namespace hedgedev::csl::ut::encoding
{
    enum encoding_type
    {
        unknown = 0 << 0,
        utf8 = 1 << 0,
        utf16_le = 1 << 1,
        utf16_be = 1 << 2,
        utf32_le = 1 << 3,
        utf32_be = 1 << 4,
        latin1 = 1 << 5
    };
}
