# Networking frames

`karkinolution/networking/frame/` owns the wire contracts and frame-level operations for system-scoped and world-scoped
messages. It converts between bytes and validated frame models; it does not own simulation state or implement domain
operations.

## Module layout

| Path | Responsibility |
|---|---|
| `base/` | Shared frame header contract, layout, and parser primitives. |
| `system/` | Server-scoped calls and outputs, including world listing, acquisition, and creation. |
| `world/` | Requests, responses, and errors for operations on an acquired world. |
| `FRAME_CONVENTIONS.md` | Wire layouts, interpreter ownership, offset rules, and frame test conventions. |

System and world scopes have separate contracts and parsers. Shared byte operations belong to
[`shared/binary/`](../../../shared/binary/), not to a frame scope.

## Responsibilities inside a scope

- `contract.hpp` defines stable frame types, subtypes, resources, and related protocol values.
- `layout.hpp` defines fixed header fields and offsets.
- `models.hpp` represents validated calls, responses, and their operation-specific metadata.
- `parser/` decodes raw bytes into frame models and routes size and subtype handling.
- `motor.hpp` and `motor.cpp` construct frames and serialize frame models.
- `interpreters/` translate simulation/domain models to and from their frame representation.
- `message_interpreters/` owns operation-specific payload codecs and protocol-only models. Keep existing entity-model
  codecs in `interpreters/`; add a message model only when the payload has a distinct or reduced representation.
- `processor.cpp` dispatches parsed calls. Payload translation and response construction belong to the server API
  binary adapter, not in the frame processor.

## Request flow

For system calls, the system parser produces a call model, the system processor dispatches it to
[`server_side/api_adapter/`](../server_side/api_adapter/), and the adapter uses the connection's `SimulationAdapter`
to perform the operation and build the output.

For world requests, the world parser produces a request model, the world processor selects a resource operation, and
the corresponding server API binary adapter translates the payload, calls the domain API, and builds the response.
Client requests and response interpretation live in [`client_side/interpreters/`](../client_side/interpreters/).

## Where to make a change

- Changing a wire code or frame shape: update the relevant scope contract, layout, model, parser/motor behavior, and
  focused protocol tests.
- Encoding an existing simulation model: use that scope's `interpreters/` and its model types.
- Encoding a request or other operation-specific payload: use that scope's `message_interpreters/`; keep its protocol
  model in the corresponding `models.hpp`.
- Connecting a server request to simulation behavior: use
  [`server_side/api_adapter/`](../server_side/api_adapter/) and keep the processor as a dispatcher.
- Reusable byte mechanics: use [`shared/binary/`](../../../shared/binary/).
- Frame-specific layouts and testing rules: read [`FRAME_CONVENTIONS.md`](./FRAME_CONVENTIONS.md).

Frame tests are organized under [`tests/karkinolution_core/networking/frame/`](../../../tests/karkinolution_core/networking/frame/).
