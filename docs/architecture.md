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

I decided that the `Id` class should carry the entity type because IDs do not exist in isolation in this project. Every ID is always associated with a specific entity type (Creature or Body).

Specifically:

* Entity → Creature or Body

# Creatures

* **Uterus** — I considered creating a dedicated class to manage uterus states, but discarded the idea because there were no more than two states at the time (YAGNI).

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

# Perception

I created a dedicated perception class to standardize the perception API and interaction patterns, avoiding direct dependence on dictionaries while keeping perception logic consistent throughout the project.
