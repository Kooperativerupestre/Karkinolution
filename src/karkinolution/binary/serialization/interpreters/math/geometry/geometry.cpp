#include "karkinolution/binary/serialization/interpreters/math/unit/unit.hpp"

#include <karkinolution/binary/serialization/interpreters/math/geometry/alias.hpp>
#include <karkinolution/binary/serialization/interpreters/math/geometry/geometry.hpp>

UnitsSRI::StandardPhysicBytes GeometrySRI::serialize_radius(GeometryForms::Radius radius) {
	return Serializer::convert_double(radius.value);
}

UnitsSRI::StandardPhysicBytes
GeometrySRI::serialize_circumference(GeometryForms::Circumference circumference) {
	return Serializer::convert_double(circumference.value);
}

UnitsSRI::StandardPhysicBytes GeometrySRI::serialize_diameter(GeometryForms::Diameter diameter) {
	return Serializer::convert_double(diameter.value);
}

UnitsSRI::StandardPhysicBytes GeometrySRI::serialize_area(GeometryForms::Area area) {
	return Serializer::convert_double(area.value);
}
