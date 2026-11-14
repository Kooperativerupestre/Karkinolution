# What is the port?

9000

Font: [Port](/src/karkinolution/networking/port.hpp)

# How do I start the server?

```bash
./build/karkinolution_app
```

How do I compile? [Commands](../../../docs/commands.md)

# What is the protocol?

[Protocol](../binary/protocol/contract.hpp)

# Does the server stay alive if the client disconnects?

Yes. A client disconnect closes only that connection; the server keeps listening and accepts new connections. Multiple
clients can connect concurrently.

# Does the server support more than one client?

Yes.

# How are connection scopes routed?

Every incoming frame starts with one scope byte. The byte is inspected by the connection's `SessionManager` and is not
forwarded to the selected protocol session. `SERVER` frames are routed to `ServerSession`; `WORLD` frames are routed to
`WorldSession` after the connection has acquired a world through the server session. A world-scoped frame received
before acquisition is a connection error and terminates that connection.

Outgoing frames are prefixed with the scope that owns them. This keeps system and world protocol frames distinguishable
without duplicating scope handling inside either protocol session.

# What does networking ownership testing cover?

Networking ownership tests verify that a world records the `ConnectionId` that acquired it and that concurrent
connections cannot acquire the same world more than once. They intentionally do not test world CRUD operations.
