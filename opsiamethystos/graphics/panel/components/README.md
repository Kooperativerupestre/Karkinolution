# Components

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

Text has a slot for spatial indexing and hit-testing and a measured current
bound for drawing. The current bound is centered on the slot and may be larger
than it: text never rejects a slot, and changing the text never fails for lack
of space, so overflow is drawn as is. This keeps what a text can do independent
of the slot of an earlier layout pass. The slot changes only through the panel.
Clipping the overflow with a scissor region is a possible future mechanic.

## Board ownership

A `Board` is a free-positioning drawing surface, not a layout container. Its
primitive drawing methods retain caller-selected positions and render in call
order, so later primitives can overlap earlier ones. A primitive is rejected
when its complete geometric bound is outside the board rectangle; touching the
boundary is valid. This gives the board a strict drawing boundary without
introducing layout policy.
