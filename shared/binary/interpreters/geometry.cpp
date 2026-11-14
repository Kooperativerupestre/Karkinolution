#include "shared/binary/interpreters/geometry.hpp"

#include "shared/binary/deserializer.hpp"
#include "shared/binary/interpreters/unit.hpp"
#include "shared/binary/serializer.hpp"

// SRI implementations
UnitsSRI::StandardPhysicBytes GeometrySRI::serialize_radius(KShared::Math::Geometry::Radius radius) {
	return KShared::Binary::Serializer::convert_double(radius.value);
}

UnitsSRI::StandardPhysicBytes
GeometrySRI::serialize_circumference(KShared::Math::Geometry::Circumference circumference) {
	return KShared::Binary::Serializer::convert_double(circumference.value);
}

UnitsSRI::StandardPhysicBytes GeometrySRI::serialize_diameter(KShared::Math::Geometry::Diameter diameter) {
	return KShared::Binary::Serializer::convert_double(diameter.value);
}

UnitsSRI::StandardPhysicBytes GeometrySRI::serialize_area(KShared::Math::Geometry::Area area) {
	return KShared::Binary::Serializer::convert_double(area.value);
}

// DSI implementations
KShared::Math::Geometry::Radius GeometryDSI::deserialize_radius(const std::vector<std::byte> &payload,
																std::size_t                   offset) {
	return KShared::Math::Geometry::Radius(KShared::Binary::Deserializer::read_double(payload, offset));
}

KShared::Math::Geometry::Circumference GeometryDSI::deserialize_circumference(const std::vector<std::byte> &payload,
																			  std::size_t                   offset) {
	return KShared::Math::Geometry::Circumference(KShared::Binary::Deserializer::read_double(payload, offset));
}

KShared::Math::Geometry::Diameter GeometryDSI::deserialize_diameter(const std::vector<std::byte> &payload,
																	std::size_t                   offset) {
	return KShared::Math::Geometry::Diameter(KShared::Binary::Deserializer::read_double(payload, offset));
}

KShared::Math::Geometry::Area GeometryDSI::deserialize_area(const std::vector<std::byte> &payload, std::size_t offset) {
	return KShared::Math::Geometry::Area(KShared::Binary::Deserializer::read_double(payload, offset));
}
