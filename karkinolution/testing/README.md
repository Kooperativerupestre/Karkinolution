# Testing support

`karkinolution/testing/` contains reusable test support, not production domain behavior. It is organized by purpose so
tests depend on the narrowest namespace that provides their setup or construction logic.

## Helpers

- `helpers/world.hpp` provides deterministic world and world-preset construction.
- `helpers/frame.hpp` builds the world frames and client request data reused by protocol and client tests.
- `helpers/shared.hpp` provides generic byte-buffer and random-string support used by Karkinolution's serialization
  tests.

Helpers use domain namespaces under `Karkinolution::Testing`; callers should not use a generic `TestUtils` namespace.

## Brokers

`brokers/` contains the small set of explicitly privileged test types that cross a production encapsulation boundary to
construct or inspect invalid states. `brokers/world.hpp` and `brokers/rtree.hpp` are friends only because their tests
must verify atomicity and recovery after a deliberately broken intermediate state. Brokers are not general helpers and
must not be used by production code or ordinary behavior tests.

## Fixtures

`fixtures/networking.hpp` contains the networking test fixture and its fake server. It owns the socket acceptor and
client `io_context` lifecycle shared by client integration tests. Domain-independent fixtures should be added here only
when they represent a reusable test lifecycle rather than a one-off setup sequence.

Test support may access private state only through an explicit friend access type whose name identifies the domain and
the testing purpose. It must not become a production API or mutate domain models merely for convenience.
