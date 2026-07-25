# Karkinolution
An individual-based artificial ecosystem simulator built in C++ with a data-oriented design.
## Overview
Karkinolution is a simulator focused on individual organisms, where creatures with unique genomes interact with the environment, reproduce, compete, and develop emergent behaviors.
Instead of modeling ecosystems only through populations or abstract mathematical models, Karkinolution focuses on the behavior and relationships of individual creatures.
The simulation is based on simple rules, perception, internal state, and decision systems. Complex behaviors emerge from the interaction between these systems rather than from hard-coded behavior scripts.
## Features
* Individual-based ecosystem simulation
* Creatures with inheritable genomes and random mutations
* Emergent behaviors through perception and decision systems
* Reproduction system with genetic inheritance
* Reactive combat based on internal state and environment
* Multiple terrain types affecting movement and resource availability
* Complete life cycle: birth, growth, reproduction, and death
* Procedural terrain generation using Perlin noise
## Creatures
Currently implemented species:
* Fish 🐟
* Crab 🦀
* Crocodile 🐊
* Hippopotamus 🦛
Each species has different traits, behaviors, and ecological interactions.
## Decision System
Creature behavior is generated through a combination of:
* Perception of the surrounding environment
* Internal states and needs
* Utility-based decision scoring
* State-based behavior transitions
The goal is not to script every action, but to create conditions where behaviors emerge naturally from the simulation rules.

## Tech Stack
* Modern C++ (C++20)
* Catch2 (unit testing)
* FastNoiseLite.h (Perlin noise terrain generation)
* GitHub Actions (CI)
## Architecture
The project follows a data-oriented design with independent and composable modules:
* `core/` — BaseStorage, Vec2, global_epsilon, LimitedValues
* `organism/` — Genetics, Creatures, Corpse, Uterus
* `brain/` — Perception, Planners, Instincts
* `systems/` — Metabolism, Reproduction, Death, Physics
* `terrain/` — Territory, Map, Soil
* `tick/` — Lifecycle & World Cycle
For architectural decisions and design rationale, see [architecture.md](docs/architecture.md).

## Credits
This project uses the following third-party libraries:
* [FastNoiseLite](https://github.com/Auburn/FastNoiseLite) — Copyright (c) 2020 Jordan Peck and Contributors. MIT License.
* [Catch2](https://github.com/catchorg/Catch2) — Copyright (c) Catch2 Authors. Boost Software License 1.0.
* [IXWebSocket](https://github.com/machinezone/IXWebSocket) — Copyright (c) Machine Zone, Inc. BSD 3-Clause License.
* [nlohmann/json](https://github.com/nlohmann/json) — Copyright (c) 2013-2026 Niels Lohmann. MIT License.

## License

Karkinolution is licensed under the Apache License 2.0.
See [LICENSE](LICENSE) for details.