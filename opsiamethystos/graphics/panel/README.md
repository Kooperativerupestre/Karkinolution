# Panel

## Component ownership and spatial index

A panel owns shared references to its components and indexes each component by
its stable component ID. The component map and quadtree represent the same
membership: adding succeeds only when the component is indexed and stored;
removing erases the component only after the index confirms removal. A
component outside the panel's rectangular boundary cannot be added.

These operations preserve consistency between the two data structures. They do
not provide synchronization between threads.

The panel owns component slot updates. It updates the quadtree and the
component together, using the previously indexed bound as the rollback source.
If either side rejects an update, the operation leaves both the component and
the quadtree unchanged.

## Collision queries

The quadtree is a broad-phase index and finds candidates by intersecting their
axis-aligned bounding boxes. Panel collision queries then test the candidates'
geometric bounds against the query shape. Results are exact with respect to
those bounds; line bounds are their conservative axis-aligned bounding boxes.

Circle queries require the full candidate search and geometric filtering,
because intersecting AABBs alone can report false positives. A query returning
one collision uses the same full candidate search before returning the first
exact match in quadtree traversal order. It is not a faster query for circles:
it must narrow AABB candidates to avoid false positives. It does not promise
the nearest component or a stable ordering.

Panels index components but do not draw them. Rendering remains the
responsibility of the screen.

## Layout slots and indexed bounds

The panel accepts rectangular layout slots for all layout-participating
components. The spatial index stores the component's conservative rectangular
axis-aligned bound, even when the component renders or tests input as another
shape. Exact collision filtering remains responsible for the component's
actual geometry. This keeps layout independent from shape-specific rendering
rules and follows the rectangular widget arrangement model used by established
UI systems.

## Declaring a layout

A layout is a tree that describes how a panel divides its space. It is plain
data: building one moves nothing. The panel applies it in `update()`. The whole
flow is three calls (`Layout` is `Opsiamethystos::Gui::Panel::Layout`):

```cpp
panel.add(label);
panel.add(button);
panel.set_layout(Layout::row({Layout::content(label, {.width = 120}),
                              Layout::fixed(100, button)},
                             {.spacing = 10}));

// Once per frame, before draw():
const auto result = panel.update();
```

`add` and `set_layout` may come in either order. A leaf that names a component
the panel does not hold is reported by `update()`, not by `set_layout`.

A `row` places its children left to right and a `column` top to bottom; that is
the main axis. Each child asks for space on it through a size policy:

- `fixed(extent, child)` takes exactly `extent`.
- `content(child, minimum)` takes the child's preferred size, never less than
  `minimum`.
- `fill(child, weight)` shares what remains after fixed and content children,
  spacing and padding, in proportion to `weight`.

`spacing` is the gap between neighbours and `padding` is the gap between the
stack's edge and its children. On the other axis, `cross_align` decides:
`STRETCH` makes every child span the stack, while `START`, `CENTER` and `END`
give each child its preferred cross size and align it. `row` and `column`
default to `STRETCH` because a nested row or column has no preferred size of
its own; any other alignment would shrink it to the minimum extent.

The layout never sees components. It asks a `Layout::Participant` for an id
and a preferred size, and `Component` implements that interface. This keeps the
layout algorithm pure and testable without components, and lets any new
component join a layout by implementing the interface. A component's preferred
size is its current bound, so a Text reports its measured text, never its slot.

Content that changes size would resize its slot with every change and move its
neighbours, for example a status label that switches text. A `content` minimum
is a floor on both axes, so the slot is the larger of the preferred size and
the minimum; choose it as the largest content the component can show.
`leaf(component, size)` goes further and ignores the preferred size, for an
extent that must not follow the content, such as a button that is always 100
high.

An invalid declaration is a programming error and throws
`std::invalid_argument`: a fixed extent that is not finite and positive, a
negative weight, spacing, padding or minimum, a child without a node, or the
same component in two leaves. `set_layout` validates before storing, so a
rejected tree keeps the previous layout. Conditions that can happen in correct
programs are reported by `update()` instead.

## Applying a layout

`update()` recomputes the layout from the components' current preferred sizes
on every call and then moves only the components whose slot changed. The panel
keeps no pending state: a new text, an added or removed component or a new
layout is picked up on the next frame without being announced, and an
unchanged frame costs arithmetic only.

The layout is never computed from the result of an earlier pass. A result that
fed back into the next input would make behavior depend on history: a text
accepted once could be rejected later because its slot shrank in between. This
is why a Text accepts any text whatever its slot (see the components
documentation). `draw()` is const and does not lay out, so call
`update()` first.

The pass is atomic. If a component cannot take its slot, every earlier change
is rolled back, the error names the first failing component, and the next call
starts from scratch. A failure therefore repeats every frame until its cause is
fixed, and what to do about it (log, throw, show a message) is the caller's
decision. `update()` returns `std::expected<LayoutReport, LayoutError>`:

- `applied` is true when the pass moved or resized at least one component.
  False means every component already had its slot, or the panel has no
  layout. It is not an error.
- `overflowed` is true when the requested sizes did not fit the panel. The
  slots are clamped inside the panel and applied anyway, so this is a problem
  with the layout's design and not a runtime failure.
- `UNKNOWN_COMPONENT`: a leaf names a component the panel does not hold, for
  example one removed after `set_layout`.
- `COMPONENT_REJECTED_SLOT`: the component refused its slot. Components
  without a rectangular slot, such as a Line, always do.
- `SLOT_OUTSIDE_PANEL` and `INDEX_REJECTED_SLOT` guard the consistency of the
  index. The layout clamps every slot to the panel, so reaching either from
  `update()` indicates a defect.

## Resizing

Resizing is an atomic layout invalidation. The panel first computes target
slots, updates components transactionally, and builds a replacement quadtree
for the new boundary. The current state is replaced only after every component
and indexed bound is valid. A resize that cannot keep every component inside
the new boundary is rejected; components are never silently removed from the
index. This preserves stable component identity while maintaining the
quadtree's containment invariant.

## Local coordinates and dragging

The panel boundary and its component slots use panel-local coordinates centered
at `(0, 0)`. The panel origin is stored separately in parent coordinates.
Dragging changes only that origin, so the quadtree remains unchanged. Parent
space hit tests subtract the accumulated panel origin before querying the local
index. This keeps movement independent from layout and avoids rewriting every
indexed component during a drag.

Parent-space collision queries use the same inverse translation as dragging.
They are the entry point for screen-level hit testing; local queries remain
available for panel-internal traversal.
