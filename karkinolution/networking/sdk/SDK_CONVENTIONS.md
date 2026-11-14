# SDK Conventions

## Interpreters

SDK interpreters are the public-facing API of the SDK. Each function in an interpreter:

1. **Builds** the binary request using the domain SRI.
2. **Sends** it via `client.request()` (blocking).
3. **Parses** the binary response using the domain DSI.
4. **Returns** the parsed response.

System and world interpreters use separate namespaces. `KarkinolutionSDK::Interpreters::System` handles scoped system
calls, such as world configuration. Existing domain interpreters are exposed below
`KarkinolutionSDK::Interpreters::World`; they retain their domain-specific behavior and use world-scoped requests. The
scope prefix belongs to the client transport and is not part of an interpreter payload.

### File layout

One `.hpp` / `.cpp` pair per domain — no subdirectory nesting.

The namespace alias is declared at the bottom of the `.hpp`, not in a separate `alias.hpp`:

```cpp
namespace CorpseSDKInterpreter = KarkinolutionSDK::Interpreters::Corpse;
```

### Namespace

`KarkinolutionSDK::Interpreters::<Domain>`

### Error handling

- Network/protocol errors surface as exceptions (propagated from `client.request()`).
- `Client::request()` and `Client::pop_response()` return either a `ResponseFrame` or an `ErrorFrame` inside the
  optional result; callers that only support response payloads surface an error frame as `ClientError`.
- A none response returns `std::nullopt` (treated the same as "none"). (only if the output can be none)
- Callers should catch `ClientError` / `ConnectionError` when resilience is needed.
