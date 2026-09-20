# Karkinolution: Godot & Core Simulator Integration Context

This document explains the architecture, communication pipeline, operational workflows, and extension patterns connecting the **Karkinolution Core Simulator** to the **Godot 4.7 Frontend**. It is designed to be an evergreen guide for developers.

---

## 1. Architectural Philosophy

Karkinolution separates the simulation world model from visualization:

```
┌─────────────────────────────────────────────────────────┐
│              Karkinolution Core Simulator               │
│  - C++20 Data-Oriented Architecture                     │
│  - Organisms, Genetics, Decisions, Terrain, Systems     │
│  - TCP Server (Asio)                                    │
│  - Binary Serialization & Deserialization Engines       │
└───────────────────────────▲─────────────────────────────┘
                            │ Raw TCP Frames
                            ▼
┌─────────────────────────────────────────────────────────┐
│          Karkinolution GDExtension Bridge               │
│  - Compiles against godot-cpp & Core libraries          │
│  - Exposes ParsedFrame, Creature, Karkinolution to ClassDB
│  - Zero duplicate parsing logic (delegates to Core)     │
└───────────────────────────▲─────────────────────────────┘
                            │ Native Godot Types (RefCounted)
                            ▼
┌─────────────────────────────────────────────────────────┐
│               Godot 4.7 Visualizer Client               │
│  - StreamPeerTCP Network Client (network_client.gd)     │
│  - Signal-driven architecture                           │
│  - Game Scenes (main.tscn / main.gd)                    │
└─────────────────────────────────────────────────────────┘
```

### Core Invariants & Rules
1. **Unidirectional Dependency**: Godot adapts to the simulator, never the other way around. The core biology, memory layouts, and physics remain completely engine-agnostic.
2. **Zero Duplicate Logic**: GDScript never parses binary bytes manually. All binary framing and payload decoding delegate to `FrameParser`, `CreatureResponseDSI`, `FrameMotor`, and `Serializer` via GDExtension.
3. **Internal Boundary**: The `godot-cpp` bindings directory is treated as an external dependency and is never manually modified or inspected.
4. **Memory Management**: Bridge models exposed to Godot inherit from `godot::RefCounted` and are passed via `godot::Ref<T>` to guarantee leak-free lifecycle management between C++ and GDScript.

---

## 2. Binary Protocol Layout

All communication over TCP uses a strict framing format:

```
┌─────────────────┬────────────────┬──────────────────────┬───────────────────────┐
│  size (4 bytes) │  type (1 byte) │  sub_type (4 bytes)  │   payload (N bytes)   │
│   std::uint32_t │  BinaryTypes   │  BinarySubTypes::*   │   std::vector<byte>   │
└─────────────────┴────────────────┴──────────────────────┴───────────────────────┘
```

- **`size`**: Number of bytes following the `size` field (`sizeof(type) + sizeof(sub_type) + payload.size()`).
- **`type`**: Packet category (`BinaryTypes`):
  - `1` = `Request` (Client $\rightarrow$ Server)
  - `2` = `Error` (Server $\rightarrow$ Client)
  - `3` = `Response` (Server $\rightarrow$ Client)
- **`sub_type`**: Message identifier defined in `src/karkinolution/binary/message_type_size.hpp`:
  - Request: `GET_CREATURE = 1`
  - Error: `CREATURE_WAS_NOT_FOUND = 1`
  - Response: `CREATURE = 0`
- **`payload`**: Serialized binary data specific to the message.

---

## 3. How the Integration Works

### A. The Core Simulator & Server (`karkinolution_app`)
- The main executable initializes a `World` with terrain, entities, and presets.
- An Asio TCP server (`Server`) listens on port `9000`.
- Incoming requests are parsed via `FrameParser`. When a `GET_CREATURE` request arrives, the server retrieves the creature from the world, serializes it using `Serializer`, and sends back a response frame.

### B. The GDExtension Bridge (`karkinolution_godot`)
Located in `src/godot/extension/`, it compiles into a shared library (`libkarkinolution_godot.so`) referenced by `karkinolution.gdextension`:

1. **`Karkinolution` (`karkinolution.hpp`)**:
   Exposes static utility methods to GDScript:
   - `build_get_creature_request(id) -> PackedByteArray`: Creates a serialized request packet.
   - `parse_frame(bytes) -> Ref<GodotParsedFrame>`: Parses raw socket bytes into a structured frame.
   - `parse_creature(payload, id) -> Ref<GodotCreature>`: Deserializes the frame payload into a `GodotCreature` instance.
2. **`GodotParsedFrame` (`binary/frame.hpp`)**:
   Strongly-typed `RefCounted` wrapper avoiding naming conflicts with `::ParsedFrame`. Holds `size`, `type`, `sub_type`, and `payload`, with helper methods `is_request()`, `is_error()`, `is_response()`, and `get_payload_string()`.
3. **`GodotCreature` (`model/creature/creature.hpp`)**:
   Godot `RefCounted` object avoiding naming conflicts with `::Creature`. Exposes properties with `const` and `[[nodiscard]]`:
   - `id`: Creature ID (`uint64_t`).
   - `gender` / `gender_name`: Numeric code and human-readable string (`MALE`, `FEMALE`).
   - `specie` / `specie_name`: Numeric code and species string (`CRAB`, `FISH`, `CROCODILE`, `HIPPOPOTAMUS`).

### C. The GDScript Client (`src/godot/karkinolution/`)
1. **`network_client.gd`**:
   - Manages connection via `StreamPeerTCP`.
   - On connect, requests creature data using `Karkinolution.build_get_creature_request(1)`.
   - Polls socket data in `_process()`.
   - Passes bytes directly to `Karkinolution.parse_frame(bytes)`.
   - Emits high-level GDScript signals:
     - `connected_to_server`
     - `connection_failed`
     - `creature_received(creature: GodotCreature)`
     - `error_received(message: String)`
2. **`main.tscn` & `main.gd`**:
   - Main scene node connects to `NetworkClient` signals via scene bindings.
   - Handlers receive typed `GodotCreature` objects and print / update visual representations.
3. **Automated Offline Tests (`tests/godot/test_karkinolution.gd`)**:
   - Standalone test harness located in the project's test suite (`tests/godot/`). Validates both offline `GodotCreature` instantiation and request/frame roundtrip serialization without requiring a running server. Executable via `ctest` or headless Godot.

---

## 4. How to Use & Run

### Step 1: Building the Targets
Use CMake to build the simulator server and the GDExtension shared library:

```bash
# Configure build directory
cmake -B build -DCMAKE_BUILD_TYPE=Debug

# Build the GDExtension library (places .so into src/godot/karkinolution/bin/)
cmake --build build --target karkinolution_godot -j$(nproc)

# Build the main simulation server
cmake --build build --target karkinolution_app -j$(nproc)
```

### Step 2: Running the Simulator Server
In a terminal, launch the server:

```bash
./build/karkinolution_app
```
The server will bind to `127.0.0.1:9000` and wait for connections.

### Step 3: Running the Godot Frontend
Open the project located at `src/godot/karkinolution` in Godot 4.7:

```bash
# Or launch directly from CLI:
godot --path src/godot/karkinolution
```
When `main.tscn` starts:
1. `NetworkClient` connects to `127.0.0.1:9000`.
2. Upon connection, it sends a `GET_CREATURE` request for ID 1.
3. The server responds with the binary frame.
4. `Karkinolution` parses the payload into a `Creature` instance and emits `creature_received`.
5. `main.gd` logs the creature's ID, Species name, and Gender.

---

## 5. How to Add New Features in the Future (Extension Recipe)

When adding a new entity or message (e.g., `WorldInfo`, `TerrainTile`, `Corpse`):

### Step 1: Define Protocol & Types in Core
1. In `src/karkinolution/binary/message_type_size.hpp`:
   - Add new enum values under `BinarySubTypes::Request` or `BinarySubTypes::Response`.
2. In `src/karkinolution/binary/`:
   - Implement the core serializer (`src/karkinolution/binary/serialization/`) or interpreter (`src/karkinolution/binary/deserialization/interpreters/`).

### Step 2: Implement the Godot Model
1. Create header & implementation under `src/godot/extension/model/<feature>/`:
   - Inherit from `godot::RefCounted`.
   - Use `GDCLASS(MyModel, godot::RefCounted)`.
   - Implement `_bind_methods()` registering properties and getters/setters.
   - Use `[[nodiscard]]` and `const` for all getters.
   - Add static factory method `from_deserialized(...)`.

### Step 3: Expose Bridge Functions
1. In `src/godot/extension/binary/parser.hpp`:
   - Add helper functions delegating to Core deserializer/FrameMotor.
2. In `src/godot/extension/karkinolution.hpp` & `karkinolution.cpp`:
   - Add static methods (e.g., `parse_my_model(...)`, `build_my_request(...)`).
   - Register them in `_bind_methods()` using `godot::ClassDB::bind_static_method`.
3. In `src/godot/extension/register_types.cpp`:
   - Register your new model class: `GDREGISTER_CLASS(MyModel)`.

### Step 4: Update Build Configuration
1. In the root `CMakeLists.txt`:
   - Add the new `.cpp` files to the `karkinolution_godot` target sources.

### Step 5: Consume in GDScript
1. In `src/godot/karkinolution/scripts/network_client.gd`:
   - Add a new signal (e.g., `signal my_model_received(data: MyModel)`).
   - In `receive_data()`, handle the new `frame.sub_type` and emit the signal.
2. Connect the signal in your scenes (`main.tscn` or specialized UI/controller scripts).
