#pragma once

#include "hash_provider.h"
#include "../thirdparty/xxHash/xxh3.h"

namespace hedgedev::csl::hash
{
    class xxhash64_provider : public hash_provider<uint64_t>
    {
    private:
        XXH3_state_t m_state{};

    public:
        xxhash64_provider();

        bool reset() override;
        bool update(const uint8_t* in_data, const size_t in_length) override;
        uint64_t digest() override;
    };
}

#include "xxhash64_provider.inl"
