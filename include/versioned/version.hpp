#pragma once

#include <chrono>
#include <cstdint>
#include <optional>
#include <string>
#include <utility>

#include "versioned/concepts.hpp"

namespace versioned {

template <Versionable T>
class Version {
public:
    Version(std::uint64_t id,
            T state,
            std::optional<std::string> message,
            std::chrono::system_clock::time_point timestamp)
        : id_(id),
          state_(std::move(state)),
          message_(std::move(message)),
          timestamp_(timestamp) {}

    [[nodiscard]] std::uint64_t id() const noexcept { return id_; }
    [[nodiscard]] const T& state() const noexcept { return state_; }
    [[nodiscard]] const std::optional<std::string>& message() const noexcept { return message_; }
    [[nodiscard]] std::chrono::system_clock::time_point timestamp() const noexcept { return timestamp_; }

private:
    std::uint64_t id_;
    T state_;
    std::optional<std::string> message_;
    std::chrono::system_clock::time_point timestamp_;
};

} // namespace versioned