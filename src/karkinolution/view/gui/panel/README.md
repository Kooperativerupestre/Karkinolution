# GUI panel

## Component ownership and spatial index

A panel owns shared references to its components and indexes each component by
its stable component ID. The component map and quadtree represent the same
membership: adding succeeds only when the component is indexed and stored;
removing erases the component only after the index confirms removal. A
component outside the panel's rectangular boundary cannot be added.

These operations preserve consistency between the two data structures. They do
not provide synchronization between threads.

Component bounds are fixed after construction, so the panel has no bounds
update operation. Moving a component requires removing it and adding a newly
constructed component.

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
