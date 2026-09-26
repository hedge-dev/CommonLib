#pragma once

#include <ranges>
#include <type_traits>
#include <vector>

#include "expr.h"

namespace hedgedev::csl::ut::expr
{
    ///
    /// Any container with vector-like behaviour.
    ///
    template <typename T>
    concept vector_like = std::ranges::contiguous_range<T> && std::ranges::sized_range<T>;

    ///
    /// Any `std::vector` type.
    ///
    template <typename T>
    concept any_vector = is_supported_template_v<T, std::vector>;
}
