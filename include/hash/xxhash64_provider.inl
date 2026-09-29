#include "../thirdparty/xxHash/xxh3.h"

namespace hedgedev::csl::hash
{
    inline xxhash64_provider::xxhash64_provider()
    {
        xxhash64_provider::reset();
    }

    inline bool xxhash64_provider::reset()
    {
        return XXH3_64bits_reset(&m_state) == XXH_OK;
    }

    inline bool xxhash64_provider::update(const uint8_t* in_data, const size_t in_length)
    {
        return XXH3_64bits_update(&m_state, in_data, in_length) == XXH_OK;
    }

    inline uint64_t xxhash64_provider::digest()
    {
        return XXH3_64bits_digest(&m_state);
    }
}
