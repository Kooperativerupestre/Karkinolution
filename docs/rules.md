# Main

This document defines the structural rules of the project that must not be violated.

# Language

English only.

# Naming

### Property

The term **Property** will be used internally as an alias for:

`method + const + [[nodiscard]]`

### Relationship Property

The term **Relationship Property** will be used internally as an alias for:

`method + object-A-const& + object-B-const& + [[nodiscard]]`

# Model vs. Physiology vs. Motor

Domains involving complex organism-related behavior **must** use the Model / Physiology / Motor structure.

This structure is not required for simple organism-related domains that do not contain complex behavior.

### Model

Contains:

* Structs & Classes
* Custom types
* Concepts

Note: Specific properties should be implemented as part of the struct in Model only when the property meets all of the
following requirements:

* It does not depend on another object to exist.
* It meets the requirements of the **Property** naming convention.
* Its calculation/get operation does not involve anything beyond the object itself, such as global values or other
  objects.

### Physiology

Contains:

* Properties
* Growth values
* Effect getters
* Relationship Properties
* Idempotent
* Deterministic

Note: **Properties** in this context refers only to properties that meet the requirements of the **Property** naming
convention but do not meet the requirements for being a Model property.

### Motor

Contains:

* Executions
* Pipelines

Motors execute actions, coordinate operations, invoke other components, use algorithms, and perform other forms of
domain execution.

### Mutation

**Motor is the only component that should have the ability to mutate domain state.**

# Other Folder Organization

## Package by Domain

The project should be organized primarily by domain rather than by technical responsibility.

# Atomicity

Every operation that depends on more than one system, must be atomic (if it's possible). Operation X:

- Operation A
- Operation B

If operation A or B fails, the state must be rolled back.

# Comments

Comments must be simple and concise. Organizational comments must be minimal and never oversized or decorative (no ASCII banner bars).

# Documentation Standards

- Format: Concise, direct, and factual. Use Q&A or structured technical outlines.
- Prohibited: Flowery pleasantries, patronizing greetings, hand-holding introductions, and emotional padding.
- Scope: Document timeless architectural patterns and rules. Never document temporary or perishable details such as active file inventories.

# File Grouping & Mirroring

- When exposing simulator models to extensions (e.g. Godot), mirror the core's file grouping.
- Do not fragment tightly coupled unit families into individual micro-files when the core simulator maintains them in a single cohesive file (e.g., keep Size and physical units together in `units.hpp`).


