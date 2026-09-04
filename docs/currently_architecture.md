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

Note: Specific properties should be implemented as part of the struct in Model only when the property meets all of the following requirements:

* It does not depend on another object to exist.
* It meets the requirements of the **Property** naming convention.
* Its calculation/get operation does not involve anything beyond the object itself, such as global values or other objects.

### Physiology

Contains:

* Properties
* Growth values
* Effect getters
* Relationship Properties

Note: **Properties** in this context refers only to properties that meet the requirements of the **Property** naming convention but do not meet the requirements for being a Model property.

### Motor

Contains:

* Executions
* Pipelines

Motors execute actions, coordinate operations, invoke other components, use algorithms, and perform other forms of domain execution.

### Mutation

**Motor is the only component that should have the ability to mutate domain state.**

# Other Folder Organization

## Package by Domain

The project should be organized primarily by domain rather than by technical responsibility.

# Atomicity

Every operation that depends on more than one system, must be atomic (if it's possible).
Operation X:
- Operation A
- Operation B

If operation A or B fails, the state must be rolled back.
