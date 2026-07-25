# Main

The language migration was carried out with careful attention to preserving the project's calculations, values, and behavior. Although some parts of the original implementation were not ideal, the main priority was to make the migration as clean and predictable as possible.

Most of the changes were related to APIs (such as LimitedValue, which was completely redesigned). Aside from these API changes, no intentional logic changes were introduced.

## Coord -> Vec2? Why?

Naming change -> I renamed it because the uses of the old Karkinolution implementation had already gone beyond just representing a "position", and it was closer to a vector containing two numbers.

Why I did not use `const` on members -> To avoid the consequences of removing behaviors and functions that occur when doing so.

Changing functions from `Vec2` to `Vec2F` -> To avoid inflating an object with map/territorial-related functions, since it can be used in other domains besides this one.

## Cell -> SoilPiece? Why?

I made this naming change to avoid ambiguity with the term "cell," which could be interpreted as referring to a biological cell. Renaming it to `SoilPiece` narrows the semantic scope and makes its purpose within the simulation more explicit.

## Why did I move the `scramble` methods to a `GenomeMotor`?

The main reason was to take advantage of C++ function overloading, making the API more idiomatic. It also prepares the genome for future, more complex operations without requiring changes to its internal logic. Finally, this aligns with the project's architectural pattern of separating data structures from the systems responsible for modifying them.

## Why did I create a safe epsilon utility?

See the `epsilon` file in the `core` module. It contains detailed comments explaining the motivation, design decisions, and the guarantees provided by the implementation.

## Algorithms

Many maximum-selection algorithms were refactored to eliminate the intermediate collection step. Instead of first storing candidates in a container and then performing a separate search for the maximum, both phases—collection and selection—were collapsed into a single loop that tracks the current maximum as it iterates. This reduces unnecessary allocations and iterations, resulting in better performance.

## Stats

I moved the `stats` module from `organism` to `core` because it is no longer used exclusively by creatures. `LimitedValue`, along with its new derived types `NormalizedValue` and `SignedNormalizedValue`, has become a fundamental part of the project, representing scores, ratios, normalized values, and other bounded quantities.

I intentionally adopted a strict approach for mathematical operations involving two `LimitedValue` instances by requiring both operands to have the same bounds. This constraint prevents accidental misuse and preserves the semantic meaning of the values being combined.

I also chose to keep `IntegerLimited` as a distinct type instead of merging it into `LimitedValue`. Integer-bounded values such as gestation time, age, and fertility represent discrete quantities with different semantics and operational requirements, so modeling them separately makes the API more explicit and reduces the likelihood of incorrect usage.

# Why Python -> C++?

Performance. In Python I hadn't yet run into fatal or annoying performance issues, but when I created the DangerIndex, I stopped to think and realized that my simulator's algorithms would probably start getting quite complex over time. But beyond that, there was also the reason that I'm planning to add a memory system for creatures, and to have searches that aren't ridiculously heavy, I need more freedom in how I structure that data — something that C++ makes easier ("zero-cost abstraction").
