#pragma once

#include <karkinolution/binary/serialization/serializer.hpp>
#include <karkinolution/math/geometry/models.hpp>

namespace GeometrySRI {
	inline constexpr std::size_t RADIUS_BYTES        = 8;
	inline constexpr std::size_t CIRCUMFERENCE_BYTES = 8;
	inline constexpr std::size_t DIAMETER_BYTES      = 8;
	inline constexpr std::size_t AREA_BYTES          = 8;

	Serializer::Types::DoubleBytes serialize_radius(GeometryForms::Radius radius);
	Serializer::Types::DoubleBytes
	serialize_circumference(GeometryForms::Circumference circumference);
	Serializer::Types::DoubleBytes serialize_diameter(GeometryForms::Diameter diameter);
	Serializer::Types::DoubleBytes serialize_area(GeometryForms::Area area);
} // namespace GeometrySRI
