#include "karkinolution/binary/interpreters/geometry.hpp"

#include "karkinolution/binary/deserializer.hpp"
#include "karkinolution/binary/interpreters/unit.hpp"
#include "karkinolution/binary/serializer.hpp"

// SRI implementations
UnitsSRI::StandardPhysicBytes
GeometrySRI::serialize_radius(Karkinolution::Math::Geometry::Radius radius) {
	return Karkinolution::Binary::Serializer::convert_double(radius.value);
}

UnitsSRI::StandardPhysicBytes
GeometrySRI::serialize_circumference(
	Karkinolution::Math::Geometry::Circumference circumference) {
	return Karkinolution::Binary::Serializer::convert_double(circumference.value);
}

UnitsSRI::StandardPhysicBytes
GeometrySRI::serialize_diameter(Karkinolution::Math::Geometry::Diameter diameter) {
	return Karkinolution::Binary::Serializer::convert_double(diameter.value);
}

UnitsSRI::StandardPhysicBytes
GeometrySRI::serialize_area(Karkinolution::Math::Geometry::Area area) {
	return Karkinolution::Binary::Serializer::convert_double(area.value);
}

// DSI implementations
Karkinolution::Math::Geometry::Radius
GeometryDSI::deserialize_radius(const std::vector<std::byte> &payload, std::size_t offset) {
	return Karkinolution::Math::Geometry::Radius(Karkinolution::Binary::Deserializer::read_double(payload, offset));
}

Karkinolution::Math::Geometry::Circumference
GeometryDSI::deserialize_circumference(const std::vector<std::byte> &payload, std::size_t offset) {
	return Karkinolution::Math::Geometry::Circumference(Karkinolution::Binary::Deserializer::read_double(payload, offset));
}

Karkinolution::Math::Geometry::Diameter
GeometryDSI::deserialize_diameter(const std::vector<std::byte> &payload, std::size_t offset) {
	return Karkinolution::Math::Geometry::Diameter(Karkinolution::Binary::Deserializer::read_double(payload, offset));
}

Karkinolution::Math::Geometry::Area
GeometryDSI::deserialize_area(const std::vector<std::byte> &payload, std::size_t offset) {
	return Karkinolution::Math::Geometry::Area(Karkinolution::Binary::Deserializer::read_double(payload, offset));
}
