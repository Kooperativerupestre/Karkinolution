# Creating a Client-Accessible API

This guide describes how to expose a Karkinolution domain operation to a remote client. The layers below have separate
responsibilities: the domain API works with domain data, binary interpreters own serialization, the API binary adapter
connects the protocol to the domain API, and client-side interpreters expose the operation to clients.

Not every domain API needs a network interface. Add the binary, server, and client-side layers only when the operation
must be available to a remote client.

## 1. Define the domain API

Start with the operation the domain needs, independent of how it will be transported.

- Place the declaration and implementation in the existing `karkinolution/api/` domain organization.
- Use the project's existing API namespace and naming conventions. Similar APIs may use different namespace styles;
  follow the conventions of the domain being changed rather than introducing a new style.
- Accept and return domain values or domain model references as appropriate. Do not expose frame types, protocol codes,
  or serialized bytes from this layer.
- When an operation may not find or retain a domain object, return `KShared::Functional::OptionalReference<const T>`.
  Use an empty reference (`nullptr`) for absence and a const reference for a present result; document whether the
  reference represents the state before or after the operation.
- Keep protocol-specific concerns, such as request IDs and response cardinality, out of the domain API.

## 2. Define the binary representation

Decide what data crosses the wire and define its binary representation separately from the domain API.

- Add or extend a frame interpreter model under `karkinolution/networking/frame/<scope>/interpreters/models/` when a
  simulation model needs a wire representation. For an operation-specific or reduced payload, use the scope's
  `message_interpreters/` and its `models.hpp`; a message model need not be a core domain type.
- Add the domain SRI and DSI under `karkinolution/networking/frame/<scope>/interpreters/`. Put request and other
  operation-specific payload codecs under `message_interpreters/`. Reuse existing mathematical and byte interpreters.
- Reuse existing unit, geometry, identity, and other interpreters instead of duplicating their encodings.
- Document shared byte and offset rules in
  [`shared/binary/BINARY_CONVENTIONS.md`](../../shared/binary/BINARY_CONVENTIONS.md), and frame layouts and ownership in
  [`FRAME_CONVENTIONS.md`](../networking/frame/FRAME_CONVENTIONS.md). Keep each wire-format decision with the protocol
  definition it governs.
- Choose stable protocol values and define the request resource, operation, response subtype, and cardinality in the
  relevant scope's `contract.hpp`. Treat these values as protocol compatibility decisions.
- Enforce subtype-specific request and response rules in the frame models, so invalid combinations cannot be
  represented as valid frames.

## 3. Connect the server to the domain API

The server path translates the request into domain inputs, calls the domain API, and translates its result back into a
binary response.

- Add an API binary adapter operation in `karkinolution/networking/server_side/api_adapter/`.
- In the adapter, interpret the request payload with the relevant DSI, call the domain API, serialize the result with
  the relevant SRI, and construct the response frame with the correct subtype and cardinality.
- Keep this binary-to-domain-to-binary transition in the adapter. Do not put protocol decoding in the domain API or
  scatter response serialization through request dispatch.
- Route the request resource and operation to the adapter through the existing frame processor.
- Define and preserve the operation's absent-result and error behavior. Use existing protocol behavior where it
  applies; do not silently turn failures into successful responses.

The API binary adapter's responsibility and rationale are described in
[`APIBYA.md`](../networking/server_side/api_adapter/APIBYA.md).

## 4. Expose the operation in the client-side interpreter

Add a domain client-side interpreter under `karkinolution/networking/client_side/interpreters/`.

- Build the request using the binary SRI and the protocol's resource, operation, target, payload, and extra fields.
- Send it through the client and use the DSI to interpret the response payload.
- Check the response subtype and cardinality expected by the operation. Surface protocol and server errors according to
  the client-side conventions; preserve the operation's defined absent-result behavior.
- Follow [`CLIENT_CONVENTIONS.md`](../networking/client_side/CLIENT_CONVENTIONS.md) for interpreter responsibilities
  and error handling.

If a client-facing view API exists for the feature, add a view querier that delegates to the client-side interpreter.
The view must not independently rebuild protocol requests or decode binary payloads.

## 5. Verify the complete path

Tests must match the claim they make.

- Protect protocol and model invariants with focused tests that exercise valid and invalid combinations, relevant
  boundaries, and all applicable finite alternatives.
- Test shared binary behavior according to [`shared/binary/BINARY_CONVENTIONS.md`](../../shared/binary/BINARY_CONVENTIONS.md)
  and frame protocols according to [`FRAME_CONVENTIONS.md`](../networking/frame/FRAME_CONVENTIONS.md).
- When claiming that a client-accessible operation works end to end, use an integration test that exercises the real
  client, transport, server dispatch, adapter, domain API, response transport, and SDK interpretation involved in that
  path. A fake client or a direct adapter call does not prove that the complete integration works.
- Add other isolated tests only when their purpose is justified with the responsible programmer or made clear in a
  concise test comment.
- Run the relevant build, tests, and code-quality checks. For tests involving threads or concurrent behavior, also run
  them with ThreadSanitizer; a Debug run alone does not establish thread safety. See
  [`HOW_TO_CREATE_TESTS.md`](../../../tests/HOW_TO_CREATE_TESTS.md) for test-purpose and test-strength guidance.

## Layer map

| Responsibility | Layer |
|---|---|
| Domain operation and domain values | Karkinolution API |
| Wire representation and payload encoding/decoding | Binary models and SRI/DSI |
| Request/response code and frame invariants | Protocol contract and frame models |
| Server request dispatch | Frame processor |
| Binary request to domain API to binary response | API binary adapter |
| Remote request and response interpretation | Client-side domain interpreter |
| Optional client-facing view query | View querier |

## Creature API tick semantics

`Karkinolution::API::Creature::tick(world, id)` executes one creature motor tick and returns an
`OptionalReference<const Creature>` to the creature's state after the tick. It returns an empty reference when the
creature does not exist before the tick or is removed during it. The const reference prevents callers from bypassing
the world and creature motors when mutating simulation state.

`Karkinolution::API::Corpse::tick(world, id)` follows the same contract for corpses: it returns the updated corpse as a
const `OptionalReference`, or an empty reference when the corpse does not exist before the tick or disappears during it.

`Karkinolution::API::Soil::tick(world, id)` follows the same contract for soil pieces: it returns the updated soil as a
const `OptionalReference`, or an empty reference when the soil piece does not exist.
