#pragma once

#include <cstdint>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

#include "versioned/concepts.hpp"
#include "versioned/version.hpp"

namespace versioned {

template <Versionable T>
class History {
public:
    explicit History(T initial) : 
    current_(std::move(initial))
    {
        versions_.emplace_back(
            0,
            current_,
            std::optional<std::string>{"Initial version"},
            std::chrono::system_clock::now()
        );
    }

    std::uint64_t commit(std::optional<std::string> message = std::nullopt) {
        versions_.erase(versions_.begin() + static_cast<std::ptrdiff_t>(currentIndex_) + 1, versions_.end());

        versions_.emplace_back(
            static_cast<std::uint64_t>(versions_.size()),
            current_,
            std::move(message),
            std::chrono::system_clock::now());

        currentIndex_ = versions_.size() - 1;
        return versions_.back().id();
    }

    void undo() {
        if (!canUndo()) throw std::logic_error("History::undo: no earlier version to undo to");
        --currentIndex_;
        current_ = versions_[currentIndex_].state();
    }

    void redo() {
        if (!canRedo()) throw std::logic_error("History::redo: no later version to redo to");
        ++currentIndex_;
        current_ = versions_[currentIndex_].state();
    }

    void checkout(std::uint64_t versionId) {
        bool found = false;
        for (int i = 0; i < versions_.size(); i++) {
            if (versions_[i].id() == versionId) {
                currentIndex_ = i;
                current_ = versions_[currentIndex_].state();
                found = true;
            }
        }

        if (!found) throw std::out_of_range("History::checkout: unknown version id");
    }

    [[nodiscard]] bool canUndo() const noexcept {
        if (currentIndex_ > 0) return true;
        return false;
    }

    [[nodiscard]] bool canRedo() const noexcept {
        if (versions_.size() - 1 > currentIndex_) return true;
        return false;
    }

    [[nodiscard]] T& current() noexcept {
        return current_;
    }

    [[nodiscard]] const T& current() const noexcept {
        return current_;
    }

    [[nodiscard]] const std::vector<Version<T>>& versions() const noexcept {
        return versions_;
    }

private:
    std::vector<Version<T>> versions_;
    T current_;
    std::size_t currentIndex_ = 0;
};

} // namespace versioned