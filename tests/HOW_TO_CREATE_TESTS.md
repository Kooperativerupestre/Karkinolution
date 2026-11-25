# How to Create Tests

## Test purpose

Every test must have a clear purpose and protect an identified behavior or invariant.

- Prefer tests that rigorously protect invariants, or integration tests that exercise a real flow through the system.
- Avoid adding lightweight, isolated tests without a clear purpose. If the goal is to prove that a feature works across components, make it a real integration test.
- Tests that do not fit these categories may still be useful when they protect meaningful behavior. Use judgment rather than treating these categories as absolute restrictions.

## Integration tests

An integration test must exercise the actual flow between the participating components. Testing a fake client, mocked server, or only one side of a new request/response path does not establish that the integration works.

For a networking flow, use the real request and response path through the client, protocol, server, request processing, and response interpretation that the feature depends on. Cover meaningful success and failure paths, including relevant protocol boundaries. Do not stop after checking that one component produced a plausible result.

## Invariant tests

Invariant tests must put the invariant under meaningful pressure rather than checking only one ordinary example.

- Cover valid and invalid states, boundaries, and all relevant alternatives in the invariant's domain.
- For protocol rules, test both acceptance and rejection. For example, exercise each applicable resource with each supported request target, and verify resource-specific restrictions for both allowed and forbidden values.
- For layout and offset rules, use prefixed sentinel bytes and verify that the decoder reads from the specified offset, not from the start of the buffer or from a mistakenly adjusted offset. Follow the domain-specific rules in [`BINARY_CONVENTIONS.md`](../karkinolution/binary/BINARY_CONVENTIONS.md).
- Prefer exhaustive checks for small finite domains. For larger domains, use deterministic generated cases and cover important edges; tests must remain reproducible.
- Assert the properties that define the invariant, not incidental implementation details.

## Test organization

- Keep tests for different domain levels or concerns in separate files. Follow the existing `tests/` directory structure and place a test alongside the closest matching domain or concern.
- Give each test suite and case a name that states the behavior or invariant being protected.
- Use the project's GoogleTest conventions and existing test helpers where applicable.
- Before introducing a new helper or fixture, check whether the shared testing infrastructure under `karkinolution/testing/helpers/` or `karkinolution/testing/fixtures/` already provides the required functionality. Reuse existing support whenever suitable.
- Avoid introducing local helpers or fixtures indiscriminately across test files. Do not duplicate functionality or create inconsistent testing conventions when shared support already exists or would be appropriate.
- If no suitable shared helper or fixture exists, determine whether the functionality should be added to the shared testing infrastructure. Request explicit authorization from the responsible programmer before introducing or expanding shared helpers or fixtures.
- Local helpers and fixtures are not inherently forbidden. They may be appropriate when the functionality is genuinely specific to a test or test file and does not warrant shared infrastructure.
- When an agent plans to introduce multiple local helpers or fixtures, it must request authorization from the responsible programmer or orchestrator before proceeding. This is intended to prevent excessive proliferation of local testing support, not to prohibit test-specific helpers.
- Use the narrowest appropriate `Karkinolution::Testing` namespace for the domain.
- Classes that deliberately bypass encapsulation to construct or inspect invalid states belong under `karkinolution/testing/brokers/` and must use the `Karkinolution::Testing::Brokers` namespace. They require an explicit friend relationship and must not be treated as ordinary helpers.
- Tests are discovered from `tests/**/*.cpp` by CMake. New C++ test files are included automatically.
- Keep comments concise. Add a comment to explain why a test exists only when its purpose is not clear from its name and assertions.

## Verification

Build the project and run the narrowest relevant tests first; run broader tests when needed to verify that behavior outside the focused test remains intact. Run style and clang-tidy checks on modified C++ files. Use the commands in [`Development Commands`](../docs/commands.md), and report exactly which checks passed.

Tests that exercise threads or concurrent behavior must also be run with ThreadSanitizer (TSAN); a passing Debug run alone is not sufficient to validate thread safety.