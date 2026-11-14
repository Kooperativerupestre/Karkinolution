# How to Documentate

## What is documentation for?

Documentation explains the reasons, invariants, boundaries, and usage contracts that are not recoverable from the
implementation alone. It should help a programmer understand a design before changing it and use a non-trivial public
or extension-facing structure safely.

## What should documentation explain?

- Why an architectural or protocol decision exists.
- Which invariant, ownership rule, atomicity guarantee, or compatibility constraint the decision protects.
- Which layer owns an operation and which layers must not bypass it.
- How an external programmer should use a non-trivial API, model, interpreter, or frame family.
- Which valid and invalid combinations the API or model enforces.

## What should documentation not do?

- Repeat class members, control flow, or signatures that the code already makes clear.
- Record temporary file inventories, current implementation details, or stale debugging history.
- Hide protocol decisions in generic prose; document them beside the protocol definition.
- Describe an operation without explaining the invariant or boundary that makes it necessary.

## Where should a decision be documented?

Document a stable decision in the narrowest relevant location:

- protocol layouts and wire compatibility in the binary conventions or protocol definition;
- public client lifecycle and usage in the client/API documentation;
- ownership, synchronization, and domain boundaries beside the responsible component;
- local rationale in a concise code comment only when it cannot be expressed more usefully in a guide.

## How should code comments be written?

Comments should explain why the surrounding code must follow a non-obvious rule. Prefer one short statement naming the
protected invariant or boundary. Do not narrate obvious statements, duplicate documentation, or use decorative banners.

## How should documentation be reviewed?

Before finalizing documentation, verify that it:

1. gives a programmer information that the code alone does not provide;
2. explains the stable reason and protected invariant;
3. identifies the correct ownership and usage boundary;
4. remains true if the implementation is refactored without changing the design.
