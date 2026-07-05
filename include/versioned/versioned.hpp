#pragma once

#include <cstdint>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

#include "versioned/concepts.hpp"
#include "versioned/history.hpp"
#include "versioned/version.hpp"
#include "versioned/diff.hpp"

namespace versioned {

template <Versionable T>
class Versioned {
public:
    explicit Versioned(T initial) : history_(std::move(initial)) {}

    T* operator->() noexcept { return &history_.current(); }
    const T* operator->() const noexcept { return &history_.current(); }
    T& operator*() noexcept { return history_.current(); }
    const T& operator*() const noexcept { return history_.current(); }

    std::uint64_t commit(std::optional<std::string> message = std::nullopt) {
        return history_.commit(std::move(message));
    }

    void undo() { history_.undo(); }
    void redo() { history_.redo(); }
    [[nodiscard]] bool canUndo() const noexcept { return history_.canUndo(); }
    [[nodiscard]] bool canRedo() const noexcept { return history_.canRedo(); }

    void checkout(std::uint64_t versionId) { history_.checkout(versionId); }

    [[nodiscard]] const std::vector<Version<T>>& history() const noexcept {
        return history_.versions();
    }

private:
    History<T> history_;
};

} // namespace versioned