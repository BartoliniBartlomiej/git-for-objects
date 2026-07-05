#include <gtest/gtest.h>

#include <chrono>
#include <optional>
#include <string>

#include "versioned/version.hpp"

namespace {

struct Point {
    int x = 0;
    int y = 0;
    bool operator==(const Point&) const = default;
};

class VersionTest : public ::testing::Test {
protected:
    std::chrono::system_clock::time_point timestamp = std::chrono::system_clock::now();
    versioned::Version<Point> version{1, Point{2, 3}, std::optional<std::string>{"start"}, timestamp};
};

} // namespace

TEST_F(VersionTest, GettersReturnIdPassedToConstructor) {
    EXPECT_EQ(version.id(), 1u);
}

TEST_F(VersionTest, GettersReturnTimestampPassedToConstructor) {
    EXPECT_EQ(version.timestamp(), timestamp);
}

TEST_F(VersionTest, GettersReturnStatePassedToConstructor) {
    EXPECT_EQ(version.state(), (Point{2, 3}));
}

TEST_F(VersionTest, MessageReturnsProvidedDescription) {
    EXPECT_EQ(version.message(), "start");
}

TEST(Version, MessageIsEmptyWhenNotProvided) {
    versioned::Version<Point> version(
        1, Point{2, 3}, std::nullopt, std::chrono::system_clock::now());

    EXPECT_FALSE(version.message().has_value());
}