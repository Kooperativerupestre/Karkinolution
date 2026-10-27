# Binary Conventions

- **DSI**: Deserializer Interpreter
- **SRI**: Serializer Interpreter
- **NLT**: Numeric Language Types
- **SO**: Serialize output
- **Endianness**: Big-endian
- **Zero-padding**: On the left

---

## Directory Structure

- **Core**:
  - `src/karkinolution/binary/serializer.[hpp|cpp]`
  - `src/karkinolution/binary/deserializer.hpp`
- **Interpreters**:
  - `src/karkinolution/binary/interpreters/<name>.[hpp|cpp]`
  - Each `<name>.hpp` contains SRI and DSI definitions, as well as the global aliases `namespace <Name>SRI` and `namespace <Name>DSI` at the bottom of the file.
  - Each `<name>.cpp` contains both SRI and DSI function implementations.
- **Interpreter models**:
  - Domain model types are declared in `src/karkinolution/binary/interpreters/models/<name>.hpp`.
  - Their namespace is `Karkinolution::Binary::Interpreters::Models::<Domain>`.
  - DSI declarations and implementations refer to these models; model types are not nested inside DSI namespaces.

---

## SRI Pattern (Serializer Interpreter)

An SRI defines binary layout protocols and converts domain types into byte sequences.

### Conventions:
1. **Namespace**: `<Domain>SRI` (e.g., `VecSRI`, `PhysicsUnitsSRI`, `StatsSRI`).
2. **Offsets**: Define field offsets as `inline constexpr std::size_t TO_GET_<FIELD>_OFFSET` **only when representing compound or aggregator types** (e.g., `Size`, `Vec3`, `Stat`). Isolated primitive values or standalone wrappers (e.g., `Radius`, `Meter`, `Circumference`) must **not** define offset constants.
3. **Fixed Buffer Types**: Define `using <Type>Bytes = std::array<std::byte, N>` for compound layouts.
4. **Protocol Documentation**: Document the exact byte layout protocol in the SRI header.
5. **Functions**: `serialize_<entity>(<DomainType>)` returning `Serializer::Types::*Bytes` or fixed array.
6. **Implementation**:
   - Primitives: delegate to `Serializer::convert_<type>()` or `Serializer::serialize()`.
   - Compounds: construct fixed array and append fields using `Deserializer::append_bytes(bytes, field_bytes, TO_GET_<FIELD>_OFFSET)`.
   - No Duplication: Do not define duplicate serializers for existing types; delegate directly to their domain SRI.

---

## DSI Pattern (Deserializer Interpreter)

A DSI reads raw byte payloads and reconstructs domain objects.

### Conventions:
1. **Namespace**: `<Domain>DSI` (e.g., `VecDSI`, `PhysicsUnitsDSI`, `StatsDSI`).
2. **No Protocol Comments**: Do not duplicate protocol layout comments in DSI; the protocol is defined in the SRI.
3. **Reuse SRI Offsets**: Never redefine offset constants in DSI. Always `#include` the corresponding SRI header and reference `<Domain>SRI::TO_GET_<FIELD>_OFFSET` for compound aggregators. Isolated values do not use offset constants.
4. **Function Signatures**: Accept payload and an optional offset (defaults to 0):
   ```cpp
   <DomainType> deserialize_<entity>(const std::vector<std::byte> &payload, std::size_t offset = 0);
   ```
5. **Implementation**:
   - Primitives: read with `Deserializer::read_<type>(payload, offset)`.
   - Compounds: delegate each field using `offset + <Domain>SRI::TO_GET_<FIELD>_OFFSET`.
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
Karkinolution::Binary::Interpreters::Models::Soil::Soil
SoilDSI::get_soil(const std::vector<std::byte> &payload, std::size_t offset) {
    return Karkinolution::Binary::Interpreters::Models::Soil::Soil{
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

- **Auxiliary getters** (`get_x`, `get_y`, `get_z`, etc.) **DO** add the field's layout offset internally. This is correct because these types are always read as a self-contained block; the caller provides only the base offset of the block.
- **The principal getter** (`get_vec`, `deserialize_size`, etc.) passes its `offset` directly to each auxiliary — the auxiliary handles its own relative position.

**Correct pattern (mid-level):**
```cpp
// Auxiliary — adds its own relative field offset
double VecDSI::get_x(const std::vector<std::byte> &payload, std::size_t offset) {
    return Deserializer::read_double(payload, VecSRI::Layout::X::OFFSET + offset);
}

// Principal — passes offset as-is; each auxiliary knows its relative position
Vec3 VecDSI::get_vec(const std::vector<std::byte> &payload, std::size_t offset) {
    return Vec3(get_x(payload, offset), get_y(payload, offset), get_z(payload, offset));
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

1. Construct a payload that serializes the full composite (e.g., a `Vec3`).
2. Call each auxiliary getter with the base offset of the block (not the field's absolute position).
3. Assert the result matches the expected field value.

### SO Pattern (Serialized output)

A SO resolves the following problem: How can we know which sub type the serialized thing has? 

#### Conventions

.payload -> Bytes
.sub_type (or sub_type())

## Frame Size Calculations

- The declared size counts bytes after the declared-size field, including the remaining fixed header and payload.
- Each request operation, response, and error has its own fixed-header size and payload-size calculation.
- `Request::<Operation>::FIXED_HEADER_SIZE` is the absolute offset of the payload start from the beginning of the
  frame. It includes the declared-size field; do not add `TOTAL_HEADER_SIZE` to it again.
- The parser-level size dispatchers receive the frame type and `Protocol::Detail::CodeVariant` subtype and route to the
  corresponding request-operation, response, or error calculation.

## Frame Detection

- `Parser::has_frame` returns `NO` for an empty buffer, `INCOMPLETE` when bytes are present but do not yet contain a
  complete frame, and `YES` when the buffer starts with a complete frame.
- Request operations expose their own `has_frame` and `get_frame` functions; request, response, and error namespaces
  dispatch frame detection by frame type and subtype.
- Frame extraction returns only the first complete frame from the buffer; buffer consumers are responsible for
  removing consumed bytes.
