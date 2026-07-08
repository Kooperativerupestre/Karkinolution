# Karkinolution

An individual-based artificial ecosystem simulator built in Python with a data-oriented design.

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

## Running the Simulator

### Installation

```bash
# Clone the repository
git clone https://github.com/Kooperativerupestre/Karkinolution.git
cd Karkinolution

# Create and activate a virtual environment
python -m venv .venv

# Linux/macOS
source .venv/bin/activate

# Windows
.venv\Scripts\activate

# Install the project and dependencies
pip install -e .
```

### Start the simulator

```bash
python -m karkinolution
```

The simulator provides two interaction modes:

* **Easy Mode** — simplified controls for users who want to observe the ecosystem with minimal configuration.
* **Hard Mode** — advanced controls with more options for creature creation, mutation, visualization, and simulation management.

### Run tests

```bash
pytest
```

## Development

The project uses a `src` layout and is installed in editable mode during development:

```bash
pip install -e .
```

This allows modifications inside `src/karkinolution` to be immediately reflected when running tests or executing the simulator.

## Tech Stack

* Python 3.10+
* pytest (unit testing)
* Rich (terminal interface)
* noise (Perlin noise terrain generation)
* GitHub Actions (CI)

## Architecture

The project follows a data-oriented design with independent and composable modules:

* `core/` — Base storage, coordinate system, and error handling
* `organism/` — Creature entities, genetics, identity, and statistics
* `decisions/` — Perception, instincts, intents, and action resolution
* `systems/` — Metabolism, reproduction, death, and physics
* `terrain/` — Terrain generation and spatial world representation
* `tick/` — Lifecycle and world cycle orchestration
* `ui/` — Rich-based terminal visualization and entry points

For architectural decisions and design rationale, see [architecture.md](docs/architecture.md).
