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