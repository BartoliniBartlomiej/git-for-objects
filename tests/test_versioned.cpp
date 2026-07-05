#include <gtest/gtest.h>

#include <string>

#include "versioned/versioned.hpp"

namespace {

struct Person {
    std::string name;
    int age = 0;
    bool operator==(const Person&) const = default;
};

} // namespace

TEST(Versioned, ArrowAndDereferenceAccessUnderlyingObject) {
    versioned::Versioned<Person> person(Person{"Bartek", 25});

    EXPECT_EQ(person->name, "Bartek");
    EXPECT_EQ((*person).age, 25);
}

TEST(Versioned, CommitUndoRedoWorkThroughFacade) {
    versioned::Versioned<Person> person(Person{"Bartek", 25});

    person->age = 26;
    person.commit("Birthday");
    EXPECT_EQ(person->age, 26);

    person.undo();
    EXPECT_EQ(person->age, 25);

    person.redo();
    EXPECT_EQ(person->age, 26);
}

TEST(Versioned, CheckoutRestoresSpecificVersionById) {
    versioned::Versioned<Person> person(Person{"Bartek", 25});
    auto initialId = person.history().front().id();

    person->age = 26;
    person.commit();
    person->age = 27;
    person.commit();

    person.checkout(initialId);

    EXPECT_EQ(person->age, 25);
}