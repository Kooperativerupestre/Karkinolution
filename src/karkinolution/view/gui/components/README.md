# GUI components

## Component invariants

A component's underlying geometric form is fixed after construction. Any
mutable component state must be separate from that form and changed through
its own interface.

Construct components in a valid state by initializing all required values at
construction. Do not rely on default construction followed by later assignment
of required state.

Components exposed through the type-erased component interface derive from
`BaseComponent`. The type-erased interface preserves the component identity
while exposing its stored alternative read-only. Layout-participating
components receive slot updates through the panel; the panel is the only owner
allowed to change their indexed bound.

Text has a maximum bound for spatial indexing and a measured current bound for
drawing and hit-testing. The current bound is centered in the maximum bound and
must always fit inside it. The maximum bound is stable between layout passes
and changes only through the panel. Updating text must leave both the old text
and current bound unchanged when the new text does not fit.
