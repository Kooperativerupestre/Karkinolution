#include <karkinolution/binary/serialization/interpreters/math/geometry/geometry.hpp>

Serializer::Types::DoubleBytes GeometrySRI::serialize_radius(GeometryForms::Radius radius) {
	return Serializer::convert_double(radius.value);
}

Serializer::Types::DoubleBytes
GeometrySRI::serialize_circumference(GeometryForms::Circumference circumference) {
	return Serializer::convert_double(circumference.value);
}

Serializer::Types::DoubleBytes GeometrySRI::serialize_diameter(GeometryForms::Diameter diameter) {
	return Serializer::convert_double(diameter.value);
}

Serializer::Types::DoubleBytes GeometrySRI::serialize_area(GeometryForms::Area area) {
	return Serializer::convert_double(area.value);
}
