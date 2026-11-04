# GUI geometry

## Position and dimensions

For every geometric form, `position` denotes its center. Dimensions describe
the form's full extents; a circle's radius is measured from its center.
Geometry calculations and rendering must follow this convention.

Containment includes the boundary: a form may touch the boundary of its
container and still be contained.

Intersection tests also include touching boundaries. Rectangle-circle
intersection uses the closest point on the rectangle; circle-circle
intersection uses the distance between centers.
