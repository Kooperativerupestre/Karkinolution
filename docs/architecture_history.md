# 1.0

# Organism

### Genetics

I decided to use a genetics storage to hold the initial genome of each species because it provides:

* Safety — A gene can only be accessed if the genome has been fully defined.
* Consistency — There is only one way to access a given gene.
* Scalability.

#### Genome Core Immutability

I decided to make the genome core immutable based on a set of design decisions:

**Vision Range** — I believe this would require a more sophisticated perception system to justify making it mutable. At the current stage of the project, it is not worth the added complexity.

**Behavior** — I considered making this mutable, but for now I want the system to be driven more by personalities and temperament than by dynamically changing weights. This allows each species to preserve its own behavioral pattern.

**Capabilities (Movement)** — Same reasoning as the vision range. There is no supporting system yet that justifies changing these values.

**Species ID** — There is no meaningful reason for this to change.

### Identity

I decided that the `Id` class should carry the entity type because IDs do not exist in isolation in this project. Every ID is always associated with a specific entity type (Creature or Corpse).

Specifically:

* Entity → Creature or Corpse

For the underlying identifier type, I chose `uint64_t` instead of a UUID, due to the lack of an available UUID library at the time.

# Creatures

* **Uterus** — Uterus state is modeled with `std::variant`, using `std::monostate` to represent the absence of a uterus.

#### Data Model

I decided to adopt a data-oriented design for all classes in this module, with the entity class acting only as a container of properties.

Although this approach introduces more verbosity in function signatures and is somewhat different from the traditional object-oriented model (where creatures "perform actions"), I found that it creates a clearer separation of responsibilities, leading to better cohesion, greater modularity, and fewer unexpected side effects. Overall, I consider it a more stable architecture.

* **ECS** — I considered using a Structure of Arrays (SoA) approach, but I did not identify a performance bottleneck significant enough to justify its implementation.

# Map

The map is split into `EntityMap` and `Territory` to avoid coupling between two fundamentally different concepts (entities and terrain cells).

Classes such as `TerrainQuery` follow the same data-oriented approach used throughout the `Creatures` module.

Both `EntityMap` and `Territory` inherit from a generic `BaseStorage` class located in the core module. I introduced this abstraction after noticing that both classes shared the exact same underlying structure, differing only in the stored types.

# Instincts

This was the most challenging part of the project's architecture.

I decided to separate the logic into a series of specialized, explicit, and organized pipelines.

Everything starts in the `Instincts` class. It first gathers all of the creature's desires (internally referred to as *intents*).

Examples include:

* `find_food`
* `find_match`
* `nothing`

This is also where hard constraints are applied. For example, if a creature is not fertile or is already pregnant, it cannot generate a reproduction intent.

If reproduction remains a valid desire, the intent is sent to the reproduction buffer stored in the `World`.

This completes the **collection phase**.

Next, a small amount of noise is applied to the intent scores.

After that, the highest-scoring intent is selected.

Each intent is then passed to its corresponding planner, which determines the concrete action the creature will execute.

Defensive attacks are implemented as an interruption that is evaluated independently to determine whether attacking is worthwhile.

*Note: `Instincts` and `Factors` now live together under `brain/instincts/`, as part of the broader `brain` module.*

# Perception

I created a dedicated perception class to standardize the perception API and interaction patterns, avoiding direct dependence on dictionaries while keeping perception logic consistent throughout the project.

*Note: `Perception` now lives under the `brain` module, alongside `Planners` and `Instincts`.*

# 1.1

## Folder Organization

During the new body-system update, it became clear that complex domains benefited significantly from a sliced architecture. This made code discovery, development, and dependency management easier.

Therefore, the project folder structure has been changed to follow this organization from now on.

## BrainGenome Created in Advance

The creature's brain is not yet complex enough to justify this abstraction. However, creating it now avoids a major structural refactoring in the future when the abstraction becomes necessary — and it will.

The same reasoning applies to the `Brain` struct within `Creature`.

## `math` Namespace

A `math` namespace was introduced. At the moment, it is essentially a wrapper around functions provided by `<cmath>` and provides no immediate practical benefit.

The purpose is to establish a stable interface so that the underlying implementation can be replaced in the future without requiring changes throughout the codebase.

## Why a Separate Container for Pathogens?

A simpler and more immediate solution would have been to include all pathogens directly in `Entity`'s `std::variant` for convenience.

However, this would combine two concepts that should remain strictly separated from the beginning: **an entity is not a pathogen**.

Pathogens therefore have their own container and are not represented as part of the `Entity` variant.

## Why Did I Make Limited Value Bounds Part of the Type?

I arrived at this decision after realizing that the `Limited*` wrappers and related types lost a significant amount of clarity because it was possible for two values with different bounds to still be the same type.

By making the constraint part of the type and therefore enforcing it at compile time, I also considered how frequently the bounds of these wrappers would actually need to change.

### Why Did I Separate Compile-Time and Runtime Bounds?

These limited values may vary depending on the requirements of the object that inherits from the wrapper. For more dynamic values, such as health, energy, and others, it does not make sense to require templates, creating a chain of template instantiations without any clear benefit.

These values can frequently change their bounds at runtime, unlike `NormalizedValue` or `LimitedValue`, whose bounds are much more stable and therefore make more sense as part of the type itself.

# 1.2

I chose R*-tree for the territory because I needed a spatial data structure that makes area-based searches, easy and efficient. Octree was chosen because I needed the same properties as R*-tree, plus one additional requirement: The creature's position is often updated, so I looked into the Octree and I realized that the insertion and updates are faster than an6 R*-tree. I'm open to changes if my reasoning is proven wrong.

