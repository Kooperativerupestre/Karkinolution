# Karkinolution Architecture

This document explains the stable boundaries between the repository's major modules and follows one request from a
client to the simulation and back. It describes responsibilities and invariants, not every current implementation
file.

## Repository boundaries

Karkinolution is organized into layers with different ownership responsibilities:

```text
shared/          KShared: reusable data types, algorithms, binary primitives, and synchronization utilities
karkinolution/   simulation models, domain logic, protocol contracts, server, and client networking
opsiamethystos/  graphics and UI primitives
app/             the application/view composition layer
tests/           unit, invariant, protocol, networking, and integration tests
```

The simulation and networking layers must not depend on the view layer. A client can observe simulation state and
request changes, but it does not mutate the server's models directly.

## KShared

`shared/` is the repository-wide foundation, exposed primarily through the `KShared` namespace. It contains reusable
building blocks that are intentionally independent of Karkinolution protocol and simulation models.

### Core

`shared/core/` provides small project-wide concepts such as IDs, colors, and base errors. These types are used by
multiple modules and should not acquire domain-specific behavior merely to serve one caller.

### Functional

`shared/functional/` provides reusable utilities whose contracts are important to higher-level code:

- `UniqueGift` and its lease type provide exclusive access to a value and release ownership through RAII.
- `OptionalReference` represents an optional reference without pretending that `std::optional<T&>` exists.
- `ScopeGuard` performs completion or rollback actions when a scope exits.
- container helpers centralize checked add, lookup, and removal operations.
- `KRandom` contains the project's pseudo-random generation and selection utilities.

These utilities protect ownership, atomicity, and completion invariants. Changes to them can affect many unrelated
domains and require focused tests.

### Math

`shared/math/` contains reusable mathematical and geometric models: vectors, geometry, physical units, statistics,
and bounded values. `LimitedValue` and runtime-configurable limited values are especially important: their invariant is
that the stored value remains inside the configured valid range. They are used throughout the simulation and should
not be changed through abrupt API migrations.

### Binary primitives

`shared/binary/` contains protocol-independent byte operations:

- serialization and deserialization of supported primitive types;
- byte ranges, append and padding helpers, and binary errors;
- reusable field/layout helpers;
- generic list and interpreter infrastructure.

It must not depend on Karkinolution frame contracts or simulation models. Protocol-specific serializers, deserializers,
layouts, and frame models belong under `karkinolution/networking/frame/` or the domain binary directories. See
[Binary Conventions](../shared/binary/BINARY_CONVENTIONS.md) for the offset and interpreter rules.

## Karkinolution core

`karkinolution/` contains the simulation and the boundaries that expose it.

### World and storage

`karkinolution/world/` owns the simulation's world model and `WorldStorage`. A world contains the domain state for a
simulation instance. `WorldStorage` uses exclusive leases so that a connected server session cannot concurrently
mutate the same world through another connection. Lease destruction returns ownership automatically.

### Domain models and execution

The domain folders describe what exists and what it can do:

- `organism/` contains organisms, entities, genetics, reproduction, and organism physiology/motors;
- `terrain/` contains terrain, soil, spatial indexes, and terrain generation;
- `world/` coordinates the complete simulation state;
- `api/` exposes stable operations over those models.

Complex organism behavior follows the Model / Physiology / Motor separation:

- Model defines state and types.
- Physiology computes deterministic properties and effects.
- Motor performs executions and mutations.

The API is the supported domain boundary for external consumers. Networking adapters call the API or the corresponding
simulation service instead of reaching into domain storage directly.

## Frames and protocol layers

A frame is the protocol contract shared by the client and server. It connects binary layout, parsing, models, request
or response construction, and processing. A typical frame domain contains:

```text
contract / aliases  public enums, IDs, variants, and domain vocabulary
layout              field offsets and fixed-size layout definitions
parser              bytes -> validated frame model
motor               model -> serialized request or response bytes
processor           validated request model -> domain operation and response model
models              typed request, response, and error data
```

The current transport has two scopes, defined in `karkinolution/networking/scope/layout.hpp`:

- `SERVER = 0`: system operations such as world discovery, creation, and configuration;
- `WORLD = 1`: operations on a world's creatures, corpses, soil, and territory.

The scope byte is transport metadata. It selects the server or world session and is not part of the scoped frame
payload parsed by that session.

## Request and response flow

The following flow describes a normal world request. System requests use the same transport but select the server
scope and system frame family.

```text
caller / commander
    -> client interpreter (SRI + frame motor)
    -> Client pending request + strand queue
    -> TCP write: scope byte + raw frame
    -> Connection read + server SessionManager
    -> ServerSession or WorldSession
    -> frame processor / API adapter / simulation
    -> response model and raw response frame
    -> TCP write
    -> Client receive + client SessionManager
    -> pending request matched by request ID
    -> client interpreter (DSI) -> caller
```

### 1. Public client operation

Client-side interpreters such as `Interpreters::Creature` build a domain payload with the domain SRI, construct the
typed world request, and call `Client::request_world_scope()`. A commander may additionally update the client's local
world representation after the authoritative response has been interpreted.

The client-side `World` is a local cache for presentation and synchronization. It is not the server's `World` and is
not a substitute for an authoritative request.

### 2. Request registration and enqueueing

`Client::request_world_scope()` validates that the client is connected, obtains a request ID, serializes the scoped
frame, registers the ID in the pending-request table, and waits for the matching response. The non-blocking
`send_without_waiting_world_scope()` performs the same registration and returns the ID immediately; callers later use
`pop_response_world_scope()`.

The raw frame is enqueued through the client's Asio strand. `Client::send()` takes the next queued frame and performs
one asynchronous write at a time. The client tracks every read and write operation, including error and cancellation
paths, so shutdown can wait until the count reaches zero.

### 3. Server transport and scope routing

`Server::accept()` creates a `Connection` for each accepted socket. `Connection::receive()` reads TCP bytes and passes
them to the server-side `SessionManager`. The manager buffers incomplete data, validates the scope byte, extracts
complete frames, and routes them:

- `SERVER` frames go to `ServerSession` and the system processor.
- `WORLD` frames go to `WorldSession`, but only after the connection has acquired a world lease.

Receiving a world frame before world acquisition is a protocol error. This protects the ownership boundary instead of
allowing an arbitrary connection to address simulation state.

### 4. Domain processing

The selected session parses the complete frame into a typed request. The frame processor selects the requested
operation and the server-side API adapter translates it into a domain API call. The simulation performs the operation
against the leased authoritative `World`, then the processor produces a typed response model.

This separation keeps transport parsing, protocol validation, and simulation mutation in distinct layers. A networking
change should not bypass the API or mutate a model directly.

### 5. Response construction and correlation

The server converts the response model into a response frame, prefixes the corresponding scope byte, and writes it on
the connection. The client receive handler continuously reads bytes and passes them to its client-side
`SessionManager`, which buffers fragmented frames, handles multiple frames in one read, parses the response, and
delivers it to the pending request identified by the request ID.

`Client::request_world_scope()` then returns the low-level response result. The interpreter checks the response kind,
converts server error frames into client errors where appropriate, treats a `NONE` response as an absent domain value,
and uses the DSI to reconstruct the domain model.

## Lifecycle and concurrency invariants

- A `Client` starts disconnected. `connect()` establishes the socket and schedules the first receive operation.
- The `io_context` must keep running while asynchronous client work is pending.
- `disconnect()` closes the socket on the client strand and fails pending requests.
- Before destroying a manually managed `Client`, callers must stop new asynchronous operations and wait for the
  operation count to reach zero. `RuntimeClient` wraps this lifecycle with its own `io_context`, worker thread, and
  RAII teardown.
- Client socket state and sending state are owned by the strand. Pending-request state has its own synchronization so
  a blocking caller can wait while the Asio thread delivers a response.
- A malformed or failed protocol operation must not leave an asynchronous-operation count or pending request
  permanently unresolved.
- The server releases a world's exclusive lease when the owning connection is destroyed.

Networking changes must be validated with ThreadSanitizer. Tests should exercise fragmented/coalesced input, malformed
frames, request correlation, disconnects, and shutdown rather than only checking a successful response.

## View and application layers

`opsiamethystos/` provides graphics and UI primitives. `app/` composes those primitives with the client-side cache and
input/application behavior. These layers may depend on client-facing APIs, but the networking and simulation layers do
not depend on rendering details.

## Where to continue reading

- [Client API and lifecycle](CLIENT.md)
- [Client-side conventions](../karkinolution/networking/client_side/CLIENT_CONVENTIONS.md)
- [Server connection and world acquisition](../karkinolution/networking/server_side/connection/README.md)
- [How to create a domain API](../karkinolution/api/HOW_TO_CREATE_API.md)
- [Binary conventions](../shared/binary/BINARY_CONVENTIONS.md)
- [Architecture history](architecture_history.md)
