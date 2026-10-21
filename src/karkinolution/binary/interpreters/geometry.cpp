#include "karkinolution/binary/interpreters/geometry.hpp"

#include "karkinolution/binary/deserializer.hpp"
#include "karkinolution/binary/interpreters/unit.hpp"
#include "karkinolution/binary/serializer.hpp"

// SRI implementations
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

// DSI implementations
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
