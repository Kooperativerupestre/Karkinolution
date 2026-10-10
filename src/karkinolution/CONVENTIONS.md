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