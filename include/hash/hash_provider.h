#pragma once

#include <type_traits>

namespace hedgedev::csl::hash
{
    template <typename T>
    class hash_provider
    {
    public:
        using type = T;
        
        virtual bool reset() = 0;
        virtual bool update(const uint8_t* in_data, const size_t in_length) = 0;
        virtual T digest() = 0;
    };

    template <typename T>
    concept hash_provider_t = requires { typename T::type; } && std::derived_from<T, hash_provider<typename T::type>>;
}
