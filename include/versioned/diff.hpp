#pragma once

#include <optional>
#include <string>

#include "versioned/concepts.hpp"
#include "versioned/version.hpp"

namespace versioned {

struct DiffResult {
    bool changed;
    std::optional<std::string> fromLabel;
    std::optional<std::string> toLabel;
};

template <Versionable T>
[[nodiscard]] DiffResult diff(const Version<T>& from, const Version<T>& to) {
    return DiffResult{
        .changed = !(from.state() == to.state()),
        .fromLabel = from.message(),
        .toLabel = to.message(),
    };
}

} // namespace versioned