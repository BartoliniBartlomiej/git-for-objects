#include <gtest/gtest.h>

#include <optional>

#include "versioned/diff.hpp"
#include "versioned/version.hpp"

namespace {

struct Point {
    int x = 0;
    int y = 0;
    bool operator==(const Point&) const = default;
};

} // namespace

TEST(Diff, ReportsNoChangeForIdenticalStates) {
    versioned::Version<Point> a(1, Point{1, 1}, std::nullopt, std::chrono::system_clock::now());
    versioned::Version<Point> b(2, Point{1, 1}, std::nullopt, std::chrono::system_clock::now());

    auto result = versioned::diff(a, b);

    EXPECT_FALSE(result.changed);
}

TEST(Diff, ReportsChangeForDifferentStates) {
    versioned::Version<Point> a(1, Point{1, 1}, std::nullopt, std::chrono::system_clock::now());
    versioned::Version<Point> b(2, Point{2, 2}, std::nullopt, std::chrono::system_clock::now());

    auto result = versioned::diff(a, b);

    EXPECT_TRUE(result.changed);
}

TEST(Diff, CarriesVersionLabelsThrough) {
    versioned::Version<Point> a(1, Point{1, 1}, std::optional<std::string>{"start"},
                                 std::chrono::system_clock::now());
    versioned::Version<Point> b(2, Point{2, 2}, std::optional<std::string>{"moved"},
                                 std::chrono::system_clock::now());

    auto result = versioned::diff(a, b);

    ASSERT_TRUE(result.fromLabel.has_value());
    ASSERT_TRUE(result.toLabel.has_value());
    EXPECT_EQ(*result.fromLabel, "start");
    EXPECT_EQ(*result.toLabel, "moved");
}