#include <gtest/gtest.h>

#include "versioned/history.hpp"

namespace {

struct Counter {
    int value = 0;
    bool operator==(const Counter&) const = default;
};

} // namespace

TEST(History, StartsWithSingleInitialVersion) {
    versioned::History<Counter> history(Counter{0});

    EXPECT_EQ(history.versions().size(), 1u);
    EXPECT_EQ(history.current().value, 0);
}

TEST(History, CommitAppendsNewVersion) {
    versioned::History<Counter> history(Counter{0});

    history.current().value = 1;
    history.commit("increment");

    EXPECT_EQ(history.versions().size(), 2u);
    EXPECT_EQ(history.current().value, 1);
}

TEST(History, UndoRevertsToEarlierState) {
    versioned::History<Counter> history(Counter{0});
    history.current().value = 1;
    history.commit();

    history.undo();

    EXPECT_EQ(history.current().value, 0);
}

TEST(History, RedoReappliesUndoneChange) {
    versioned::History<Counter> history(Counter{0});
    history.current().value = 1;
    history.commit();
    history.undo();

    history.redo();

    EXPECT_EQ(history.current().value, 1);
}

TEST(History, CommitAfterUndoDiscardsFutureVersions) {
    versioned::History<Counter> history(Counter{0});

    history.current().value = 1;
    history.commit();              // v1
    history.current().value = 2;
    history.commit();              // v2
    history.undo();                // back to v1

    history.current().value = 5;
    history.commit();              // delete v2, add new v2

    EXPECT_EQ(history.versions().size(), 3u); // initial, v1, new v2 - NO 4
    EXPECT_EQ(history.current().value, 5);
}

TEST(History, UndoWithoutHistoryThrows) {
    versioned::History<Counter> history(Counter{0});

    EXPECT_THROW(history.undo(), std::logic_error);
}