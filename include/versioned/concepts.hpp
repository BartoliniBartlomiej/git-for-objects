#pragma once

#include <concepts>

namespace versioned {

template <typename T>
concept Versionable = std::copy_constructible<T> && std::equality_comparable<T>;

} // namespace versioned