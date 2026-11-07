# 1.0 - Python version

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

# 1.1 - C++ version (after migration)

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

# 1.2 - After great reorganization of project

I chose R*-tree for the territory because I needed a spatial data structure that makes area-based searches, easy and efficient. Octree was chosen because I needed the same properties as R*-tree, plus one additional requirement: The creature's position is often updated, so I looked into the Octree and I realized that the insertion and updates are faster than an6 R*-tree. I'm open to changes if my reasoning is proven wrong.
### Why did I create a R*tree service?

Because I realized that some invariants don't just belong to the territory. Some concepts that I defined as requiring atomicity are required for all structures that use an R*tree with a Registry.

(Note: I couldn't separate the grass feature and the R*tree service feature)

### Why did I create a PerceptionView instead of having only Perception?

First, in my old python project I had just the perception. Over time, I realized that copying a dict whenever we realize an analysis on the perception uses too much memory because of the allocation of hashmaps. So the decision was to split the responsabilities between Perception and PerceptionView. The perception is the source of truth, and it should never be modified after being created; the PerceptionView only references the things in the perception -- wich avoids making copies.

### Why did I choose uint64_t for BaseIdType?

Because it can represent a large range.

# 1.3 - Connecting to Godot

### Server

#### Why only one client?

Because I don't have any needy to justificate more than one client.

### Why did I change int64_t for BaseIdtype? 

Because the >0 restriction of uint64_t was possibly dangerous when it would be sent do Godot.

### Why did I choose Godot for the interface?

First, it's equally important say which alternatives I considered.

#### TUI (terminal)

I already had some experience with TUI when I was working on the Python version of _Karkinolution_. I used the Rich library. And I liked it, but quickly the sensation of the game started to become more crammed as the complexity of the entities grew. 

#### Own engine

Based on my goals, an engine isn't something _necessary_, nor is it aligned with the project's vision.

#### Unity

I didn't consider about.

#### Unreal

The trade-off between complexity and more advanced graphics and features wasn't worth it for my goals. Given that my mainly goal was only a _visualizer_ of the simulation. 

#### Godot

I chose it because I wanted a simple engine, that I could connect to C++ and build 2D/2.5D views. And I think that Godot really offers to this project the simplicity and portability that the project needs.

##### Why didn't I make the simulation inside Godot?

Because I wanted to actually keep the simulation separated and view as it was in Python. Karkinolution must be able to run independently of the visualization.

#### Why did I choose 2.5D instead of 3D camera?

My main inspiration is worldbox and I actually appreciate this type of cammera. 

#### Who makes the sprites?

The owner of this project. There are no artists.

# 1.4 - A new beginning, 🐦‍🔥

## Why did I change my opinion about Godot?

Because when I was writing about Godot in 1.3 chapter, I hadn't worked with godot-cpp. However, I realized today that I should investigate again whether Godot is really necessary. The main reason behind the choice of Godot was that I needed a visualizer, but an engine as Godot brings more infrastructure than Karkinolution actually needs. 

## Why did I switch to Raylib?

Raylib. I chose it because I needed an extensible UI that I could enhance whenever I wanted. I also considered ImGui, but it is too simple for what I need. From what I understood, Raylib offers OpenGL integration and more customizable components, while ImGui is more focused on and used for HUDs and debug interfaces, which is not my case — I needed an extensible interface so I could make it look better whenever I wanted, even if it means having more things on my hands.

I added raygui because I did not want to reinvent GUI elements from scratch. That is not the project's main goal and doing so would significantly slow development.

## Why did I remove ImGui and rlImGui?

I did not like ImGui's system. Its way of working was so far from what I intended for Karkinolution that the cost of building an abstraction on top of ImGui's abstractions was equal to or greater than the cost of creating my own components.

# 1.5 - C++26 toolchain

## Why did I standardize on Clang 21 with GCC 16's libstdc++?

The project now targets C++26, so the compiler and standard library must be
selected as one tested toolchain. Clang 21 provides the project compiler while
GCC 16 provides the libstdc++ implementation required by C++26 facilities such
as `std::inplace_vector`. Docker and CI use the same combination and verify the
selected GCC installation before building.

`std::inplace_vector` is not being applied mechanically to existing vectors.
The view's entity and patch collections still need explicit domain capacity
decisions before they can safely use fixed-capacity storage.

## Why I created the Karkinolution::Functional?

Because I wanted to express some built-in utilities that the C++ Standard Library doesn't include. Also, K_random will be moved into Karkinolution::Functional.

## Why I removed FrameFieldExistence from every has_* in Frames::Parser?

It wasn't consumed by any system, therefore only introduced more maintenance overhead.

## Binary Frame Analysis and SDK Results

### Frame size and operation-specific layouts

Each request operation has its own fixed-header size because `SEE` includes a target field while the other request
operations do not. `FIXED_HEADER_SIZE` is the absolute offset from the beginning of the frame to the payload; it
already includes the declared-size field.

The declared size counts bytes after the declared-size field. Payload-size calculations therefore use the fixed header
for the specific request operation, response, or error, rather than assuming one shared header size.

### Frame detection

The parser's `has_frame` result distinguishes three buffer states: `NO` means no bytes are available, `INCOMPLETE`
means bytes are present but do not yet contain a complete frame, and `YES` means a complete frame is available.
Request-operation namespaces, Response, and Error each provide frame detection; operation-specific namespaces also
extract their frame. This lets the buffer analyzer use frame availability directly instead of separately checking
size, type, subtype, and payload.

Frame extraction returns the first frame from the buffer. The buffer consumer removes exactly the bytes belonging to
that frame, leaving any subsequent frame data available for the next analysis.

### SDK response and error results

The SDK's pending-request result can contain either a `ResponseFrame` or an `ErrorFrame`. `Protocol::Detail::ResponseVariant`
remains the variant of response/error subtype codes; a separate frame variant carries the complete parsed frames.
Keeping both alternatives in the result allows callers to receive server errors without misrepresenting an error frame
as a response frame.

SDK domain interpreters that return a specific domain object accept response frames and surface error frames as
`ClientError`, since an error frame does not contain the requested domain object.

### SDK soil-list interpretation

The SDK decodes collection responses through the shared list deserializer instead of duplicating list-size and
element-offset parsing in each domain interpreter. The soil interpreter uses the soil protocol's fixed serialized
element size to convert each list entry into the SDK soil model.

### TCP response buffering

TCP reads provide arbitrary chunks of a byte stream, not complete protocol frames. The client therefore keeps reading
and accumulating bytes until the frame parser can extract a complete frame. The socket read chunk size is separate
from the maximum accumulated frame size: the former controls each asynchronous read, while the latter bounds a full
frame. The current 64 KiB frame-buffer limit accommodates larger collection responses without relying on one read
containing the entire response.

# 1.6 - Structural namespace refactor

## Why did I remove muscle and skeleton from creatures?

I realized that I had added muscle and skeleton to the model simply because I could, without a concrete reason or any
real use for them—not even now. Since there was no compelling reason to keep them, I used the structural namespace
refactor as an opportunity to remove them as well.

## Why did I remove transformations from Karkinolution's genetics?

For the same reason: I thought transformations would make the system easier to use, but when I actually needed them,
they turned out to be an almost unreadable abstraction. They added overhead and made the very thing I wanted to
simplify more difficult.

# 1.7 - Removing BaseStorage

## Why did I remove `BaseStorage`?

`BaseStorage` originated in the Python version of Karkinolution, where the dictionary interface felt too limited and
did not provide accessible, consistent APIs for some common operations. During the migration to C++, I carried the
class over without reconsidering the key question: was this abstraction still necessary?

It was not. Keeping `BaseStorage` in C++ added indirection on top of `std::unordered_map` without providing enough
value to justify it. The `try_add`, `try_at`, and `try_remove` operations remain useful, so they now live as
`Karkinolution::Functional` helpers that operate directly on `std::unordered_map`.

# 1.8 - Moving the simulation from 3D to 2D

## Why did I remove the simulation's third dimension?

The visual engine being developed for Karkinolution is currently a 2D engine. A simulation that models height and
three-dimensional spatial relationships introduced disproportionate complexity without providing an emergent behavior
or gameplay advantage that the project currently needs. The simulation therefore keeps only two spatial coordinates,
while the visualization can evolve independently when it needs richer presentation.

The goal of this migration was to remove the simulation's 3D behavior without changing unrelated creature, terrain,
protocol, or lifecycle behavior.

## Migration plan

The work was intentionally staged so each architectural layer could be validated before the next one was changed:

1. Move the former 3D mathematical types and 3D spatial tests to `misc/3d/`. This directory is a historical archive and
   is not included in the build.
2. Recreate the active mathematical layer as 2D equivalents.
3. Migrate organism positions, sizes, embryo area, and related genetic/core data.
4. Replace the active Octree path with the existing Quadtree implementation under
   `src/karkinolution/functional/quadtree.hpp`, and archive the former Octree implementation and tests.
5. Migrate the R*-tree and its terrain service from three-dimensional boxes to two-dimensional boxes.
6. Migrate the binary protocol and its tests after the simulation core was migrated.

The implementation was compiled with a maximum build parallelism of three jobs during the migration. Each stage was
stopped for review before proceeding to the next stage.

## Mathematical decisions

The active vector type is now `Vec2` with `x` and `y` coordinates. Three-dimensional-only operations, including the
third coordinate and the active fixed-Z helper, were removed. The old implementation remains available only in the
3D archive for historical reference.

`Size` now contains `Width width` and `Height height`. `Height` is the second planar extent of a rectangle; it does not
represent a restored world-elevation coordinate. The former `Lateral` name was replaced with `Width` to make the 2D
meaning explicit. `Volume` and `SharedVolume` became `Area` and `SharedArea`, and area is calculated as width times
height.

Embryos preserve their area while deriving a two-dimensional `Size`. This keeps the existing area-based growth model
while removing the third extent.

## Spatial structure decisions

The active spatial index is now a generic functional Quadtree. The view's existing quadtree implementation was moved
to `src/karkinolution/functional/quadtree.hpp`, and the view panel now uses that shared implementation. The former
Octree implementation and its tests were moved to `misc/3d/` and are not compiled.

The R*-tree now uses `Box2D` and `Box2DMotor`. Its ordering, intersection, enlargement, margin, and area calculations
operate only on X/Y bounds. The R*-tree service, soil conversion, terrain generation, perception queries, and related
tests were migrated to the same two-dimensional representation. Tree `depth` values remain because they describe
hierarchical tree levels, not a spatial coordinate.

## Binary protocol decisions

The binary protocol follows the active 2D model instead of retaining unused padding for the removed dimension:

* `Vec2` serializes X and Y only, using 16 bytes.
* `Size` serializes width and height only, using 16 bytes.
* `Area` and `SharedArea` replace volume-based names and serializers.
* Creature, corpse, soil, and territory layouts were shortened to remove Z/depth fields.
* DSI offset responsibility remains unchanged: high-level getters receive absolute field offsets, while mid-level
  composite getters add their own relative offsets.

Roundtrip and offset-invariant tests were updated to assert the new layouts. The binary protocol remains big-endian and
keeps its existing frame and request semantics; only spatial payload fields changed.

### Protocol compatibility

This migration is a breaking protocol change for spatial payloads. Clients and servers built against the former 3D
protocol are not wire-compatible with the 2D protocol because `Vec2` and `Size` have fewer serialized fields and the
domain layouts have different offsets and total sizes. No compatibility aliases, padding fields, or dual 3D/2D
decoders were retained in the active code. Deployments must update both ends of the connection together.

# Shared Layer Extraction

I intend to separate Karkinolution's visual engine into an independent folder. To make this possible, I needed to isolate the dependencies that intersected both the simulation and the engine simultaneously. My solution was to create this `shared` directory containing all of these reused files.
