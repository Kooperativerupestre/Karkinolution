# Binary Conventions

- **DSI**: Deserializer Interpreter
- **SRI**: Serializer Interpreter
- **NLT**: Numeric Language Types
- **Endianness**: Big-endian
- **Zero-padding**: On the right

---

## Directory Structure

- **SRI**: `src/karkinolution/binary/serialization/interpreters/<domain>/<name>.[hpp|cpp]`
- **DSI**: `src/karkinolution/binary/deserialization/interpreters/<domain>/<name>.[hpp|cpp]`

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