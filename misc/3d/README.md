# Archived 3D components

This directory preserves the former 3D simulation components for historical
reference.

The simulation is moving from three-dimensional data to a two-dimensional
model because the additional complexity of 3D does not provide a current
benefit to the Karkinolution visual engine. These files are intentionally
outside the project's source tree and are not included in the build.

The archived components may still refer to the former source layout and are
not maintained as part of the active simulation. The former 3D math, Octree
implementation, and Octree tests are preserved here. Future 2D replacements
are implemented in the active source tree.

The archive is not a compatibility layer. New code must use the active 2D
types and structures rather than importing or restoring these archived 3D
definitions.
