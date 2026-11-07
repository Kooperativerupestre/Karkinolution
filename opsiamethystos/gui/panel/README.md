# GUI panel

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
