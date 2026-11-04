# GUI components

## Component invariants

A component's underlying geometric form is fixed after construction. Any
mutable component state must be separate from that form and changed through
its own interface.

Construct components in a valid state by initializing all required values at
construction. Do not rely on default construction followed by later assignment
of required state.

Components exposed through the type-erased component interface derive from
`BaseComponent` and provide an immutable maximum bound. The type-erased
interface preserves that bound and the component identity while exposing its
stored alternative read-only.

Text has a fixed maximum bound for spatial indexing and a measured current
bound for drawing and hit-testing. The current bound is centered in the maximum
bound and must always fit inside it. Updating text must leave both the old text
and current bound unchanged when the new text does not fit.
