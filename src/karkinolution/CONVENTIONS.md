# Ids

All ids must have the type int64_t with these bounds: 1 < id < max(int64_t)

# Global unit of measurement

Meter.

# What is the biggest size of the things?

This size was chosen based on:

- Godot representation
- Simulator possibilities
- Entertainment

10KM.

# What is the type for physic things?

double.

# Why the underlyign type of Vec3 in godot is float instead of double?

Because there is no loss based on the biggest size of the territory and things. And Godot in my simulator only exists to display the stuff -- it doesn't have to calculate or modify anything.

# What is the role of godot?

"Visualize without modifying directly. Only requests changes."

# Naming Conventions

| Element | Convention | Example | Notes |
|---|---|---|---|
| Variables | `snake_case` | `creature_speed` | |
| Functions | `snake_case` | `calculate_speed()` | |
| Methods | `snake_case` | `get_creature()` | |
| Classes | `PascalCase` | `Creature` | |
| Structs | `PascalCase` | `CreatureData` | |
| Namespaces | `PascalCase` | `Karkinolution::SDK` | |
| Concepts | `PascalCase` | `Serializable` | |
| Type aliases | `PascalCase` | `CreatureId` | |
| Constants | `UPPER_SNAKE_CASE` | `MAX_CONNECTIONS` | |
| Enumeration types | `PascalCase` | `RequestType` | |
| Enumerators | `UPPER_SNAKE_CASE` | `REQUEST`, `RESPONSE` | |
| Member variables | `snake_case_` | `creature_speed_` | Preferred style. `snake_case` and `_snake_case` are also accepted. No convention is strictly established. |
| Template type parameters | `PascalCase` or single uppercase letter | `ValueType`, `T` | Both styles are accepted. |
| Non-type template parameters | `PascalCase` | `MaxSize` | |
| Preprocessor macros | `UPPER_SNAKE_CASE` | `MAX_BUFFER_SIZE` | |
| File names | `snake_case` | `creature_manager.cpp`, `creature_manager.hpp` | Applies to headers and source files. |
