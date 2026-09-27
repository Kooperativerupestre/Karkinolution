#include "karkinolution/binary/deserialization/interpreters/math/geometry/geometry.hpp"

#include <karkinolution/binary/deserialization/deserializer.hpp>

GeometryForms::Radius GeometryDSI::deserialize_radius(const std::vector<std::byte> &payload,
													  std::size_t                   offset) {
	return GeometryForms::Radius(Deserializer::read_double(payload, offset));
}

GeometryForms::Circumference
GeometryDSI::deserialize_circumference(const std::vector<std::byte> &payload, std::size_t offset) {
	return GeometryForms::Circumference(Deserializer::read_double(payload, offset));
}

GeometryForms::Diameter GeometryDSI::deserialize_diameter(const std::vector<std::byte> &payload,
														  std::size_t                   offset) {
	return GeometryForms::Diameter(Deserializer::read_double(payload, offset));
}

GeometryForms::Area GeometryDSI::deserialize_area(const std::vector<std::byte> &payload,
												  std::size_t                   offset) {
	return GeometryForms::Area(Deserializer::read_double(payload, offset));
}
