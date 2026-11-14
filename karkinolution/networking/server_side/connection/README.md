# Server-Side Connection & World Acquisition

## Overview

The server connection infrastructure manages client sockets, scopes inbound and outbound binary frames, and enforces
exclusive, single-lease ownership over simulation worlds.

## Frame Scopes

Every wire frame is prefixed by a single-byte scope identifier:

- **`SERVER` (`0x01`)**: System-level handshakes and administrative actions (e.g., world enumeration and configuration).
  Routed directly to `ServerSession`.
- **`WORLD` (`0x02`)**: Domain entity requests (creatures, corpses, soil, territory). Routed to `WorldSession`.

## World Acquisition Model

Worlds are stored in `WorldStorage` encapsulated within `UniqueGift<World>`. This design enforces exclusive ownership
through `GiftLease<World>`: at most one active connection may hold a lease on a given world at any time.

### Acquisition Flow

1. **Initial State**: A new `Connection` starts unassociated with any world (`world_lease = std::nullopt`).
2. **Scope Guard**: While unassociated, `SessionManager` rejects all inbound `WORLD`-scoped frames with an exception.
3. **Configuration Request**: The client sends a `SERVER`-scoped `CONFIG` frame specifying the target `WorldId`.
4. **Lease Negotiation**:
   - `ServerSession` calls `SimulationAdapter::config()`, which requests `Simulation::take_world(world_id, connection_id)`.
   - `WorldStorage::take(world_id)` checks the entry state. If `NOT_TAKEN`, it grants a `GiftLease<World>` and transitions the entry to `TAKEN`.
   - If the world does not exist or is already `TAKEN` by another connection, the request fails (`could_connect = false`).
5. **Session Activation**:
   - On successful acquisition, the `GiftLease<World>` is preserved inside `ServerSession`.
   - `SessionManager` detects the acquired lease and instantiates `WorldSession(server_session.world())`.
   - `WORLD`-scoped frames are now unlocked and dispatched to `WorldSession`.

## Teardown & RAII Invariants

- When a connection disconnects, drops, or encounters a protocol error, the `Connection` and its session managers are destroyed.
- Destroying `ServerSession` drops the `GiftLease<World>`.
- The `GiftLease` RAII destructor automatically releases exclusive ownership back to `WorldStorage`, returning the entry to `NOT_TAKEN` without requiring manual server cleanup.
