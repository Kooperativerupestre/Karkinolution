# Karkinolution

A creature ecosystem simulator with emergent behaviors, 
built in Python with a data-oriented architecture.

## Overview

Karkinolution simulates an ecosystem where creatures with unique genomes 
interact, reproduce, and compete for resources across different terrains. 
Emergent behavior arises from simple rules governing each entity — no 
hard-coded behavior scripts.

The architecture is data-oriented: creatures are composed of independent 
components (genome, diet, combat stats), making the addition of new 
behaviors and entity types surgical, without breaking existing systems.

## Features

- Creatures inherit traits from parents with random mutation, influencing 
  diet, aggressiveness, and reproductive capacity
- Reactive combat system — creatures attack based on proximity and 
  internal state, not fixed scripts
- Multiple terrain types affect mobility and resource availability
- Full life cycle: birth, growth, reproduction, and death

## Quick Start

```bash
# 1. Clone the repository
git clone https://github.com/Kooperativerupestre/Karkinolution.git
cd Karkinolution

# 2. Install dependencies
pip install -r docs/requirements.txt

# 3. Run the simulator
python main.py
```

## Tech Stack
- Python 3.10+
- pytest (unit tests)
- Rich (terminal UI)
- GitHub Actions (CI/CD)
- noise (perling noise)

## Architecture
Data-oriented design with independent, composable modules:
- `core/` — Base storage, coordinate system, error handling
- `organism/` — Creature entities (genetics, identity, stats)
- `decisions/` — Perception, instincts, intent, and action resolution
- `systems/` — Metabolism, reproduction, death, physics
- `map/` — Terrain generation and spatial world representation
- `tick/` — Lifecycle and world cycle orchestration
- `ui/` — Rich-based terminal visualization and entry points

See [architecture.md](docs/architecture.md) for design decisions and rationale.