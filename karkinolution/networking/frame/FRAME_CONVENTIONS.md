# Frame Conventions

- **DSI**: Deserializer Interpreter
- **SRI**: Serializer Interpreter
- **NLT**: Numeric Language Types
- **SO**: Serialize output
- **Endianness**: Big-endian
- **Zero-padding**: On the left

---

## Directory Structure

Frame contracts, layouts, models, parsers, and motors live under `karkinolution/networking/frame/`, grouped by scope:
`base/`, `system/`, and `world/`. Within system and world scopes, `interpreters/` serialize simulation models, while
`message_interpreters/` own operation-specific payloads and protocol-only models. Reuse an existing model when it
represents the exchanged data; define a message model only when the payload has a distinct or reduced shape.

### Shared binary primitives

Reusable byte primitives live under `shared/binary/` in the `KShared::Binary` namespace. This layer contains byte-range
constraints, byte append/padding helpers, generic serialization/deserialization, binary errors, supported-type
constraints, field layout helpers, and type-name helpers. It must not depend on protocol contracts or frame models, so
the core, networking, client-side, and other modules can use the same binary mechanics.

Protocol-specific aliases, validators, numeric-type mappings, interpreters, and frames belong under
`karkinolution/networking/frame/` and the relevant Karkinolution domain modules. They may depend on `KShared::Binary`,
but shared binary primitives must not depend on frame contracts or domain models.

- **Core**:
  - `shared/binary/serializer.[hpp|cpp]`
  - `shared/binary/deserializer.hpp`
- **Generic binary types**:
  - `shared/binary/types/function_concepts.hpp`
  - `shared/binary/types/list.hpp`
  - Generic list serialization belongs to `KShared::Binary::Types`; it must not depend on Karkinolution protocol
    contracts or domain models.
- **Interpreters**:
  - Frame model interpreters: `karkinolution/networking/frame/<scope>/interpreters/<name>.[hpp|cpp]`
  - Message payload interpreters: `karkinolution/networking/frame/<scope>/message_interpreters/<name>.[hpp|cpp]`
  - Shared mathematical interpreters: `shared/binary/interpreters/<name>.[hpp|cpp]`
  - Keep SRI and DSI declarations and implementations together by interpreter/domain, following the surrounding module's
    namespace and alias conventions.
- **Interpreter models**:
  - Frame representations of simulation models are declared under
    `karkinolution/networking/frame/<scope>/interpreters/models/`.
  - Message-only models are declared beside their operation codecs, commonly in
    `karkinolution/networking/frame/<scope>/message_interpreters/models.hpp`.
  - World frame model namespace is `Karkinolution::Networking::Frames::World::Interpreters::Models::<Domain>`.
  - Shared mathematical model namespace is `KShared::Binary::Interpreters::Models::<Domain>`.
  - Model types are not nested inside SRI or DSI namespaces.

---

## SRI Pattern (Serializer Interpreter)

An SRI defines binary layout protocols and converts domain types into byte sequences.

### Conventions:
1. **Namespace**: `<Domain>SRI` (e.g., `VecSRI`, `PhysicsUnitsSRI`, `StatsSRI`).
2. **Field Layouts**: Define ordered fields in `SRI::Layout` with `KShared::Binary::Field` and `KShared::Binary::Next`.
   Reuse these aliases for both serialization and deserialization instead of duplicating offsets.
3. **Fixed Buffer Types**: Define `using <Type>Bytes = std::array<std::byte, N>` for fixed-size layouts.
4. **Protocol Documentation**: Document the exact byte layout protocol in the SRI header.
5. **Functions**: Accept the corresponding model and return its fixed byte array or variable-length byte sequence.
6. **Implementation**:
   - Primitives: delegate to `Serializer::convert_<type>()` or `Serializer::serialize()`.
   - Compounds: construct a fixed array and append fields with
     `KShared::Binary::Utils::append_bytes(bytes, field_bytes, Layout::Field::OFFSET)`.
   - No Duplication: Do not define duplicate serializers for existing types; delegate directly to their domain SRI.

---

## DSI Pattern (Deserializer Interpreter)

A DSI reads raw byte payloads and reconstructs domain objects.

### Conventions:
1. **Namespace**: the corresponding DSI namespace or alias (e.g., `CreatureDSI`, `SoilDSI`, `VecDSI`).
2. **No Protocol Comments**: Do not duplicate protocol layout comments in DSI; the protocol is defined in the SRI.
3. **Reuse SRI Layouts**: Never redefine field offsets in DSI. Include the corresponding SRI header and reference
   `<Domain>SRI::Layout::<Field>::OFFSET`.
4. **Function Signatures**: Accept payload and an optional offset (defaults to 0):
   ```cpp
   <DomainType> deserialize_<entity>(const std::vector<std::byte> &payload, std::size_t offset = 0);
   ```
5. **Implementation**:
   - Primitives: read with `Deserializer::read_<type>(payload, offset)`.
   - Compounds: delegate each field using `offset + <Domain>SRI::Layout::<Field>::OFFSET`.
   - No Duplication: Do not define duplicate deserializers for existing types; delegate directly to their domain DSI.

---

## DSI Offset Responsibility Rule

DSI getters follow a strict rule about **who is responsible for adding the field offset within a payload**. The rule differs by domain level:

### High-Level Domain DSIs (e.g., `SoilDSI`, `CorpseDSI`, `CreatureDSI`)

These represent the top-level serialized entities exchanged over the wire.

- **Auxiliary getters** (`get_id`, `get_position`, `get_type`, etc.) must **NOT** add the field's layout offset internally. They receive an absolute `offset` that already points to the start of the field they are responsible for reading.
- **The principal getter** (`get_soil`, `get_corpse`, `get_creature`) is the **only one** responsible for computing `offset + Layout::Field::OFFSET` before delegating to each auxiliary getter.

**Correct pattern (high-level):**
```cpp
// Auxiliary — reads at the exact offset it receives, no field offset added
BaseIdType SoilDSI::get_id(const std::vector<std::byte> &payload, std::size_t offset) {
    return Deserializer::deserialize<BaseIdType>(payload, offset);
}

// Principal — adds field offset before calling auxiliaries
Karkinolution::Networking::Frames::World::Interpreters::Models::Soil::Soil
SoilDSI::get_soil(const std::vector<std::byte> &payload, std::size_t offset) {
    return Karkinolution::Networking::Frames::World::Interpreters::Models::Soil::Soil{
        .id = get_id(payload, offset + SoilSRI::Layout::Id::OFFSET),
        // ...
    };
}
```

**Wrong pattern (do NOT do this):**
```cpp
// Auxiliary adds its own field offset — broken if called from within get_soil
BaseIdType SoilDSI::get_id(const std::vector<std::byte> &payload, std::size_t offset) {
    return Deserializer::deserialize<BaseIdType>(payload, SoilSRI::Layout::Id::OFFSET + offset);
}
```

### Mid-Level Composite DSIs (e.g., `VecDSI`, `UnitsDSI`, `GeometryDSI`)

These represent shared mathematical/geometric types that do not belong to a single high-level domain and are composed internally (i.e., their fields have a fixed relative layout).

- **Auxiliary getters** (`get_x`, `get_y`, etc.) **DO** add the field's layout offset internally. This is correct because these types are always read as a self-contained block; the caller provides only the base offset of the block.
- **The principal getter** (`get_vec`, `deserialize_size`, etc.) passes its `offset` directly to each auxiliary — the auxiliary handles its own relative position.

**Correct pattern (mid-level):**
```cpp
// Auxiliary — adds its own relative field offset
double VecDSI::get_x(const std::vector<std::byte> &payload, std::size_t offset) {
    return Deserializer::read_double(payload, VecSRI::Layout::X::OFFSET + offset);
}

// Principal — passes offset as-is; each auxiliary knows its relative position
Vec2 VecDSI::get_vec(const std::vector<std::byte> &payload, std::size_t offset) {
    return Vec2(get_x(payload, offset), get_y(payload, offset));
}
```

---

## Unit Test Requirements for DSIs

Every DSI and its individual getter functions **must** have unit tests that verify the offset responsibility rule described above.

For **high-level domain DSIs**, tests must prove that auxiliary getters do **not** automatically add their field offset. The canonical test pattern is:

1. Construct a synthetic payload with known **garbage/sentinel bytes** before the actual field data.
2. Call the auxiliary getter with the exact offset pointing to the field start (i.e., garbage bytes + field position).
3. Assert the result matches the expected field value — proving the getter read from the correct position and did not double-add an internal offset.

For **mid-level composite DSIs**, tests must prove that auxiliary getters **do** add their field offset internally when given the base offset of the composite block:

1. Construct a payload that serializes the full composite (e.g., a `Vec2`).
2. Call each auxiliary getter with the base offset of the block (not the field's absolute position).
3. Assert the result matches the expected field value.

### Serialized frame outputs

A serialized frame output has a subtype in its frame header and a payload encoded by the matching SRI. Keep subtype
selection in the frame motor/processor path and payload encoding in its owning interpreter; do not encode the subtype
again inside the payload.

## Create-world system call

`SystemCall::CREATE_WORLD` is a non-generic system-call subtype. Its fixed header extends the base system-call header
with one boolean byte, `AutoConnect`, immediately after the call id. Its payload is the serialized `WorldPreset`:

- `seed`: 4 bytes;
- `size`: the existing KShared `Size` interpreter layout (16 bytes);
- `epsilon`: 4 bytes.

The server creates the world and, when `AutoConnect` is true, acquires its lease for the requesting connection while
holding the shared simulation mutex. This makes creation and connection one atomic operation with respect to other
connections. The response payload is a separate one-byte boolean indicating whether creation succeeded.

## Create-creature request payload

`CREATE CREATURE` has an 87-byte fixed payload. Optional values are represented by a one-byte presence flag followed by
their reserved field slot:

| Offset | Field | Size |
|---:|---|---:|
| 0 | Has gender | 1 |
| 1 | Gender | 1 |
| 2 | Has species | 1 |
| 3 | Species | 1 |
| 4 | Has position | 1 |
| 5 | Position (`Vec2`) | 16 |
| 21 | Has name | 1 |
| 22 | Name size | 1 |
| 23 | Name slot | 64 |

The protocol `CreaturePreset` contains only the client-selectable values in this payload. It is not a second domain
creature model; the server passes its decoded values to the existing creature creation API. The name size must not
exceed the fixed name slot.

## Frame Size Calculations

- The declared size counts bytes after the declared-size field, including the remaining fixed header and payload.
- Each request operation, response, and error has its own fixed-header size and payload-size calculation.
- `FIXED_HEADER_SIZE` constants in a scope's `layout.hpp` are the absolute payload-start offsets from the beginning of
  the frame. They include the declared-size field; do not add the total header size to them again.
- Scope-specific parser dispatchers route by frame type and subtype to the matching request, response, or error size
  calculation.

## Creature cycle operation

`CYCLE CREATURE <id>` uses the existing eight-byte creature ID payload and the `CYCLE` request operation. The server
executes one creature motor tick through `Karkinolution::API::Creature::tick`. A present creature is returned as a
`CREATURE` response with cardinality `ONE`; an unknown or removed creature returns a `NONE` response.

`CYCLE SOIL <id>` follows the same layout and response cardinality. The server executes one soil motor tick and returns
the updated soil as a `SOIL` response, or `NONE` when the soil ID is absent.

## Domain-specific payload layouts

For `SEE ALL CREATURE`, `SEE ALL CORPSE`, and `SEE ALL SOIL`, the response payload is a shared binary list whose
elements are only the resource IDs. Each element is one `int64_t` ID; the response does not serialize the complete
entity. This keeps enumeration separate from fetching mutable entity state through the corresponding `SEE ONE`
operation.

The `CREATURE` response layout, its grouped transport data, field offsets, and total size are documented in the
`CREATURE PROTOCOL` block in [`world/interpreters/creature.hpp`](world/interpreters/creature.hpp). This document
defines reusable frame rules and protocol decisions; the owning SRI remains the source of truth for entity layouts.

## Frame Detection

- `Parser::has_frame` returns `NO` for an empty buffer, `INCOMPLETE` when bytes are present but do not yet contain a
  complete frame, and `YES` when the buffer starts with a complete frame.
- Request operations expose their own `has_frame` and `get_frame` functions; request, response, and error namespaces
  dispatch frame detection by frame type and subtype.
- Frame extraction returns only the first complete frame from the buffer; buffer consumers are responsible for
  removing consumed bytes.

## Related documentation

- [Frame directory guide](README.md)
- [System frame contract](system/contract.hpp)
- [World frame contract](world/contract.hpp)
- [Shared binary conventions](../../../shared/binary/BINARY_CONVENTIONS.md)
- [Architecture and request flow](../../../docs/ARCHITECTURE.md)
