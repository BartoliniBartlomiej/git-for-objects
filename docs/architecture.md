# Architecture

## Constraint that shaped everything: no inheritance, no macros

The brief requires the library to version **arbitrary** C++ types without
requiring them to inherit from a base class or use registration macros.
With no base class to lean on, requirements on `T` can't be expressed
through virtual methods - they have to be expressed structurally, at
compile time.

```cpp
template <typename T>
concept Versionable = std::copy_constructible<T> && std::equality_comparable<T>;
```

Any existing type that is already copyable and comparable works with the
library immediately - zero changes needed in the user's type. This is
the same philosophy behind `std::hash` or `std::less`: express
requirements structurally, not through a common ancestor.

## Three classes, three responsibilities

- **`Version<T>` - Memento.** An immutable snapshot of `T` plus metadata
  (id, timestamp, optional commit message). Enforced immutability: private
  fields, only `const`-returning getters, no setters. This matters because
  a "checkpoint in history" that could be silently mutated later would
  defeat the entire purpose of versioning.

- **`History<T>` - Caretaker.** Owns the ordered list of `Version<T>` and
  a cursor (`currentIndex_`) pointing at the active one, plus a separate
  mutable `current_` field holding the "working copy" the user actually
  edits (`person->age = 26`) before committing. All version-control state
  machine logic (`commit`, `undo`, `redo`, `checkout`) lives here, tested
  independently of the facade.

- **`Versioned<T>` - Facade.** The only class most users interact with
  directly. Adds pointer-like ergonomics (`operator->`, `operator*`) and
  forwards every other operation straight to `History<T>` - no duplicated
  logic.

Keeping `History<T>` separate from `Versioned<T>` means the state machine
(the hard part) can be unit tested without needing to go through the
facade's ergonomics at all.

## Commit semantics: why `commit()` after `undo()` discards the future

This mirrors Git: if you check out an older commit and then commit again,
the previously "newer" commits become unreachable from the current branch
tip. `History::commit()` implements this by erasing every version after
the current cursor position before appending the new one.

This is exactly why `checkout()` is a *separate* operation from
`commit()`: `checkout()` never discards anything (you can freely jump
around existing history), only `commit()` does, because only `commit()`
creates new history that needs the old "future" branch cleared out of the
way first.

## Diff: intentionally simple for now

`diff()` currently answers a single question - "did the state change
between these two versions?" - based on `T::operator==`:

```cpp
template <Versionable T>
DiffResult diff(const Version<T>& from, const Version<T>& to) {
    return DiffResult{ .changed = !(from.state() == to.state()), ... };
}
```

A field-level diff was considered and rejected for now: doing it
generically for *any* `T` without reflection (not yet ergonomic
pre-C++26), and without requiring extra boilerplate from the user (which
would violate the "no extra requirements" goal), is a project of its own.
The type is deliberately a plain aggregate (`struct` with public fields,
no methods) since it holds data with no invariant to protect - unlike
`Version<T>`, which must stay immutable.

## Planned, not yet built

- **JSON serialization.** Will live in its own header
  (`serialization/json_serializer.hpp`), separate from the core
  (`versioned.hpp`). The core will have zero dependency on JSON - a user
  only pays for the `nlohmann::json` dependency if they actually
  `#include` the serialization header. A `Serializable<T>` concept will
  mirror `Versionable<T>`: satisfied automatically if `T` already has
  ADL-found `to_json`/`from_json`, the same convention `nlohmann::json`
  itself uses.

- **Pluggable storage.** `FullSnapshotStorage<T>` (full copy of `T` per
  version) is the only strategy today. It's already factored out as its
  own type rather than inlined into `History`, specifically so that a
  future `DeltaStorage<T>` (storing only the diff against the previous
  version) could be substituted via a template parameter
  (`History<T, StoragePolicy>`) without touching `History`'s logic. This
  matters for large objects (e.g. a CAD document with thousands of
  elements) committed frequently - full-copy storage is the right
  default for simplicity, but doesn't scale indefinitely.

## Explicitly out of scope

| Feature | Why excluded now | How it would slot in later |
|---|---|---|
| Branches / merge | Needs a version tree, not a flat vector, plus conflict resolution rules | `History<T>` would track a tree instead of `std::vector`; `Versioned<T>`'s public API would be largely unaffected |
| Field-level diff | Needs reflection or per-type opt-in boilerplate | Additional ADL customization point for `diff()`, `DiffResult` gaining an optional field |
| Concurrent/multi-user history | Out of scope for a single-process portfolio library | `History<T>` would need internal synchronization; public API would not need to change |

The recurring theme: the public API of `Versioned<T>` is kept small and
stable so each of the above is an *additive* change, not a breaking one.