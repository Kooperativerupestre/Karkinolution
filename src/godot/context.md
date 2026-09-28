# Godot Integration Architecture

## Q: What is the end-to-end data flow from Godot bytes to storage?
1. `NetworkTransport` reads raw bytes from `StreamPeerTCP`.
2. Raw bytes pass to `FrameParser::parse_frame()`, returning a framed packet.
3. Domain API inspects the frame type/subtype and invokes the conversion namespace.
4. The conversion namespace invokes the simulator's existing deserializer (DSI), which parses the binary payload into raw values.
5. The conversion namespace populates an immutable Godot DTO struct with those values.
6. The resulting Godot DTO is saved into `GodotEntityStorage` by ID.

## Q: How are Godot models decoupled from simulator core models?
Godot models must never include or depend on simulator core headers. They are completely standalone Godot `RefCounted` structs. A dedicated conversion namespace bridges the two: it calls the simulator deserializers (DSI) and passes the extracted primitive data into the Godot structs.

## Q: Can Godot structs have behavior or mutation methods?
No. Godot model structs are strictly immutable Data Transfer Objects (DTOs) for transport. They contain zero setters, zero mutating operations, and zero domain logic. They expose only `const` `[[nodiscard]]` getters.

## Q: Why collapse LimitedValue into GodotLimitedValue?
Compile-time template classes cannot be registered dynamically into Godot's `ClassDB`. All bounded/normalized templates collapse into `GodotLimitedValue` with dynamic `value`, `min`, `max` properties. Domain-specific metrics (`GodotSharedVolume`, `GodotEfficiency`, `GodotQuality`) subclass `GodotLimitedValue` with fixed bounds to preserve semantic typing in GDScript.

## Q: What is the pattern for adding a new type to Godot?
1. **Core**: Ensure the simulator has a deserializer function in its DSI namespace.
2. **Godot Model**: Create an independent, immutable `RefCounted` DTO with const getters and a static factory. Do not include simulator headers.
3. **Conversion**: In the conversion namespace, add a function that calls the core DSI and passes the resulting values to the Godot DTO factory.
4. **Registration**: Register the Godot class in `register_types.cpp` via `GDREGISTER_CLASS`.

## Q: Why are spatial data structures (R*-tree, Octree) omitted from the Godot client?
Spatial index performance bottlenecks and spatial queries are strictly resolved server-side in the simulation engine. The Godot client only handles presentation, rendering, and state storage; duplicating complex spatial structures like R*-trees or Octrees on the client introduces unnecessary overhead and synchronization complexity. The Godot-side `GodotTerrain` fuses the territory's dimensions (`GodotSize`) and soil collection without replicating server spatial trees.

