# How to Create Tests

## Test purpose

Every test must have a clear purpose and protect an identified behavior or invariant.

- Prefer tests that rigorously protect invariants, or integration tests that exercise a real flow through the system.
- Do not add a lightweight, isolated test merely because a behavior seems useful to check. If the goal is to prove
  that a feature works across components, make it a real integration test.
- If a proposed test is neither a substantial invariant test nor a real integration test, justify why it should exist.
  Discuss the need with the responsible programmer, Koöperative Rüpestrën, or explain the reason in a concise comment
  in the test.

## Integration tests

An integration test must exercise the actual flow between the participating components. Testing a fake client,
mocked server, or only one side of a new request/response path does not establish that the integration works.

For a networking flow, use the real request and response path through the client, protocol, server, request processing,
and response interpretation that the feature depends on. Cover meaningful success and failure paths, including
relevant protocol boundaries. Do not stop after checking that one component produced a plausible result.

## Invariant tests

Invariant tests must put the invariant under meaningful pressure rather than checking only one ordinary example.

- Cover valid and invalid states, boundaries, and all relevant alternatives in the invariant's domain.
- For protocol rules, test both acceptance and rejection. For example, exercise each applicable resource with each
  supported request target, and verify resource-specific restrictions for both allowed and forbidden values.
- For layout and offset rules, use prefixed sentinel bytes and verify that the decoder reads from the specified offset,
  not from the start of the buffer or from a mistakenly adjusted offset. Follow the domain-specific rules in
  [`BINARY_CONVENTIONS.md`](../src/karkinolution/binary/BINARY_CONVENTIONS.md).
- Prefer exhaustive checks for small finite domains. For larger domains, use deterministic generated cases and cover
  important edges; tests must remain reproducible.
- Assert the properties that define the invariant, not incidental implementation details.

## Test organization

- Keep tests for different domain levels or concerns in separate files. Follow the existing `tests/` directory
  structure and place a test alongside the closest matching domain or concern.
- Give each test suite and case a name that states the behavior or invariant being protected.
- Use the project's GoogleTest conventions and existing test helpers where applicable.
- Tests are discovered from `tests/**/*.cpp` by CMake. New C++ test files are included automatically.
- Keep comments concise. Add a comment to explain why a test exists only when its purpose is not clear from its name
  and assertions.

## Verification

Build the project and run the narrowest relevant tests first; run broader tests when needed to verify that behavior
outside the focused test remains intact. Run style and clang-tidy checks on modified C++ files. Use the commands in
[`Development Commands`](../docs/commands.md), and report exactly which checks passed.

Tests that exercise threads or concurrent behavior must also be run with ThreadSanitizer (TSAN); a passing Debug run
alone is not sufficient to validate thread safety.
