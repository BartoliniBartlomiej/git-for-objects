# git for objects

A modern, header-only C++20 library for Git-like versioning of arbitrary
objects: commit checkpoints, undo/redo, and diff between versions -
without requiring your types to inherit from anything or use any macros.

```cpp
#include <versioned/versioned.hpp>

struct Person {
    std::string name;
    int age = 0;
    bool operator==(const Person&) const = default;
};

versioned::Versioned<Person> person{Person{"Bartek", 25}};

person->age = 26;
person.commit("Birthday");

person.undo();   // age back to 25
person.redo();   // age back to 26

for (const auto& version : person.history()) {
    // version.id(), version.message(), version.timestamp(), version.state()
}
```

Any type is versionable as long as it is copyable and equality-comparable -
that's it. No base class, no registration macro.

## Why this exists

Undo/redo and change-history logic tends to get reimplemented ad hoc in
every project that needs it (editors, CAD tools, configuration systems,
business apps). This library factors that logic out into a reusable,
type-safe component that works with existing value types as-is.

## Building

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

Dependencies (currently: GoogleTest) are fetched automatically via CMake
`FetchContent` - no manual setup required beyond a C++20 compiler and
CMake 3.20+.

## API overview

| Type | Role |
|---|---|
| `versioned::Versioned<T>` | Public facade. Pointer-like access (`operator->`, `operator*`) plus `commit`, `undo`, `redo`, `checkout`, `history()`, `diff()`. |
| `versioned::History<T>` | Owns the ordered list of versions and the "current" cursor. `Versioned<T>` delegates all version-control logic here. |
| `versioned::Version<T>` | An immutable snapshot of `T` plus metadata: id, timestamp, optional commit message. |
| `versioned::diff(...)` | Compares two `Version<T>` and reports whether the underlying state changed. |

See [`docs/architecture.md`](docs/architecture.md) for the design
rationale behind each of these (Memento pattern, why concepts instead of
inheritance, commit semantics, etc.).

## Design highlights

- **No inheritance, no macros.** Requirements on `T` are expressed as a
  C++20 `concept` (`Versionable`: copyable + equality-comparable), not a
  base class.
- **Git-like commit semantics.** `commit()` after `undo()` discards the
  now-unreachable "future" versions, exactly like committing after
  checking out an older commit in Git. `checkout()`, by contrast, never
  discards anything.
- **Small, focused classes.** `Version` (Memento), `History` (Caretaker),
  `Versioned` (Facade) each have one job and are tested independently.

## Project status

This is a portfolio project focused on clean architecture and modern
C++ (concepts, templates, RAII) rather than feature completeness.

Implemented so far:
- Commit / undo / redo / checkout with Git-like history semantics
- Simple diff (state-changed detection)

Planned:
- JSON serialization of version history (opt-in module, no dependency
  pulled into the core unless used)
- Pluggable storage policy (full snapshot today; delta storage as a
  future drop-in replacement)

Deliberately out of scope for now (see `docs/architecture.md` for how
the architecture would accommodate them later):
- Branches and merging, with conflict detection
- Field-level diff
- Concurrent/multi-user history

## License

MIT.