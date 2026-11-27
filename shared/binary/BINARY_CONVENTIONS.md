# Binary Conventions

This document defines the reusable binary rules shared by serializers, deserializers, interpreters, and protocol
frames. It explains ownership and offset responsibilities; exact domain payload layouts belong beside the SRI that
defines them.

## Vocabulary

- **SRI**: Serializer Interpreter; converts a typed value or frame model into bytes.
- **DSI**: Deserializer Interpreter; reconstructs a typed value or frame model from bytes.
- **NLT**: Numeric Language Type, the project's typed numeric wrappers.
- **SO**: serialized output, the byte representation produced by an SRI.
- **Byte range**: a contiguous byte container accepted by a parser or deserializer.

The wire representation uses big-endian byte order and left zero-padding for fixed-width numeric values unless a
protocol-specific layout explicitly states otherwise.

## Ownership boundaries

### Shared binary primitives

`shared/binary/` belongs to `KShared::Binary`. It contains protocol-independent mechanics:

- `serializer.hpp/.cpp` and `deserializer.hpp`;
- byte range concepts, byte append/padding helpers, and binary errors;
- supported-type and numeric-type constraints;
- field/layout helpers and type-name utilities;
- generic list serialization and shared mathematical interpreters.

This layer must not depend on Karkinolution frame contracts, network scopes, or simulation models.

### Protocol and domain binary code

Protocol-specific layouts, aliases, validators, frame parsers, motors, and models belong under
`karkinolution/networking/frame/` and the related Karkinolution binary/domain directories. They may depend on
`KShared::Binary`, but the dependency must point in one direction.

An interpreter header normally contains both its SRI and DSI declarations, with the public aliases declared at the
bottom of the file. Domain model types belong in the corresponding `models/` namespace rather than inside an
interpreter namespace.

## Primitive serialization

Primitive serializers should delegate to the shared serializer for the type rather than duplicating byte conversion.
Primitive deserializers should use the matching `Deserializer::read_<type>()` operation and preserve the requested
signedness. Adding a supported primitive or wrapper requires tests for its boundary values and round trip.

The binary layer validates buffer sizes before reading or writing. A parser must never read beyond the supplied byte
range, and a malformed or incomplete range must produce the project's binary error or an incomplete-frame result as
appropriate.

## SRI conventions

An SRI owns the layout it serializes.

1. Use a domain-specific SRI namespace, such as `VecSRI`, `PhysicsUnitsSRI`, or `StatsSRI`.
2. Define `TO_GET_<FIELD>_OFFSET` only for compound or aggregate layouts. Standalone primitive wrappers do not need
   offsets.
3. Define fixed-size byte aliases for fixed-size compound values.
4. Document the exact field order, width, and meaning beside the SRI declaration.
5. Return the existing shared byte types when possible instead of inventing duplicate containers.
6. For compound values, append each field at the SRI-owned offset and delegate field serialization to that field's
   existing SRI.

An SRI is the source of truth for its layout. A DSI must reuse its offsets rather than redefining them.

## DSI conventions

DSIs accept a payload and an optional base offset, normally defaulting to zero:

```cpp
DomainType deserialize_domain(const std::vector<std::byte> &payload, std::size_t offset = 0);
```

Primitive getters read with the matching shared deserializer. Compound getters delegate each field to its DSI and
apply the offset according to the responsibility rules below.

### High-level domain DSIs

High-level DSIs represent complete wire entities such as `Soil`, `Corpse`, and `Creature`.

- Auxiliary getters (`get_id`, `get_position`, `get_type`, and similar) do **not** add their own field offset. The
  caller supplies the absolute offset of the field.
- The principal getter (`get_soil`, `get_corpse`, `get_creature`) adds the SRI field offset before delegating to each
  auxiliary getter.

```cpp
BaseIdType SoilDSI::get_id(const std::vector<std::byte> &payload, std::size_t offset) {
    return Deserializer::deserialize<BaseIdType>(payload, offset);
}

Soil SoilDSI::get_soil(const std::vector<std::byte> &payload, std::size_t offset) {
    return Soil{
        .id = get_id(payload, offset + SoilSRI::Layout::Id::OFFSET),
    };
}
```

An auxiliary getter that adds the field offset again would double-apply it whenever the principal getter supplies an
absolute field position.

### Mid-level composite DSIs

Mid-level DSIs represent self-contained reusable blocks such as `Vec2`, `Size`, and `Geometry`.

- Auxiliary getters (`get_x`, `get_y`, and similar) **do** add their relative field offset internally.
- The principal getter passes the block's base offset unchanged to those auxiliary getters.

```cpp
double VecDSI::get_x(const std::vector<std::byte> &payload, std::size_t offset) {
    return Deserializer::read_double(payload, offset + VecSRI::Layout::X::OFFSET);
}

Vec2 VecDSI::get_vec(const std::vector<std::byte> &payload, std::size_t offset) {
    return Vec2{get_x(payload, offset), get_y(payload, offset)};
}
```

The distinction is intentional: high-level entity callers pass absolute field positions, while mid-level composite
callers pass the base of a reusable block.

## Testing offset responsibility

Every DSI and individual getter must have tests that protect its offset contract.

For high-level DSIs:

1. Place sentinel or garbage bytes before the target field in a synthetic payload.
2. Call the auxiliary getter with the exact absolute field offset.
3. Assert that the expected value is read. This catches accidental internal offset addition.

For mid-level composite DSIs:

1. Serialize a complete composite block at a non-zero base offset.
2. Call each auxiliary getter with the base offset, not the field offset.
3. Assert that the expected field values are read. This catches missing internal relative offsets.

Tests should cover insufficient buffers, boundaries, signedness, zero values, and representative non-zero offsets.

## Frame and payload sizing

Frame protocols add their own headers around binary payloads. The declared-size field counts bytes after that field,
including the remaining fixed header and payload. Each request, response, and error family owns its fixed-header and
payload-size calculation.

`Request::<Operation>::FIXED_HEADER_SIZE` is the absolute payload-start offset from the beginning of the frame. It
already includes the declared-size field; callers must not add the total header size a second time.

Parser-level size dispatchers receive the frame type and subtype and route to the corresponding operation's size
calculation. Exact operation layouts remain in the owning frame SRI or protocol header.

## Frame buffering and detection

TCP reads are arbitrary chunks, so frame consumers must support both fragmented frames and multiple frames in one read.
Frame detection follows this contract:

- an empty buffer returns `NO`;
- bytes that do not yet contain a complete frame return `INCOMPLETE`;
- a buffer beginning with a complete frame returns `YES`;
- frame extraction returns the first complete frame only;
- the buffer owner removes consumed bytes and keeps trailing bytes for the next frame.

Scope bytes and frame bytes have separate responsibilities. The networking scope router consumes the scope byte, and
the selected scoped session parses only the frame bytes that follow it.

## Domain-specific layouts

Do not duplicate domain payload layouts in this document. The SRI that owns a layout must document its field order,
widths, offsets, cardinality, and compatibility requirements. This document supplies the reusable rules that all such
layouts must follow.

## Related documentation

- [System frame contract](../../karkinolution/networking/frame/system/contract.hpp)
- [Frame directory guide](../../karkinolution/networking/frame/README.md)
- [Frame conventions](../../karkinolution/networking/frame/FRAME_CONVENTIONS.md)
- [Architecture and request flow](../../docs/ARCHITECTURE.md)
