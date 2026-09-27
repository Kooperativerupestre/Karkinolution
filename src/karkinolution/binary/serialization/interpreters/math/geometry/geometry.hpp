#pragma once

#include <karkinolution/binary/serialization/serializer.hpp>
#include <karkinolution/math/geometry/models.hpp>

namespace GeometrySRI {
	Serializer::Types::DoubleBytes serialize_radius(GeometryForms::Radius radius);
	Serializer::Types::DoubleBytes serialize_circumference(GeometryForms::Circumference circumference);
	Serializer::Types::DoubleBytes serialize_diameter(GeometryForms::Diameter diameter);
	Serializer::Types::DoubleBytes serialize_area(GeometryForms::Area area);
} // namespace GeometrySRI
