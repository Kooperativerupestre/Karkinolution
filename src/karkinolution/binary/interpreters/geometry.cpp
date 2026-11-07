#include "karkinolution/binary/interpreters/geometry.hpp"

#include "karkinolution/binary/deserializer.hpp"
#include "karkinolution/binary/interpreters/unit.hpp"
#include "karkinolution/binary/serializer.hpp"

// SRI implementations
UnitsSRI::StandardPhysicBytes GeometrySRI::serialize_radius(KShared::Math::Geometry::Radius radius) {
	return Karkinolution::Binary::Serializer::convert_double(radius.value);
}

UnitsSRI::StandardPhysicBytes
GeometrySRI::serialize_circumference(KShared::Math::Geometry::Circumference circumference) {
	return Karkinolution::Binary::Serializer::convert_double(circumference.value);
}

UnitsSRI::StandardPhysicBytes GeometrySRI::serialize_diameter(KShared::Math::Geometry::Diameter diameter) {
	return Karkinolution::Binary::Serializer::convert_double(diameter.value);
}

UnitsSRI::StandardPhysicBytes GeometrySRI::serialize_area(KShared::Math::Geometry::Area area) {
	return Karkinolution::Binary::Serializer::convert_double(area.value);
}

// DSI implementations
KShared::Math::Geometry::Radius GeometryDSI::deserialize_radius(const std::vector<std::byte> &payload,
																	  std::size_t                   offset) {
	return KShared::Math::Geometry::Radius(Karkinolution::Binary::Deserializer::read_double(payload, offset));
}

KShared::Math::Geometry::Circumference
GeometryDSI::deserialize_circumference(const std::vector<std::byte> &payload, std::size_t offset) {
	return KShared::Math::Geometry::Circumference(
		Karkinolution::Binary::Deserializer::read_double(payload, offset));
}

KShared::Math::Geometry::Diameter GeometryDSI::deserialize_diameter(const std::vector<std::byte> &payload,
																		  std::size_t                   offset) {
	return KShared::Math::Geometry::Diameter(Karkinolution::Binary::Deserializer::read_double(payload, offset));
}

KShared::Math::Geometry::Area GeometryDSI::deserialize_area(const std::vector<std::byte> &payload,
																  std::size_t                   offset) {
	return KShared::Math::Geometry::Area(Karkinolution::Binary::Deserializer::read_double(payload, offset));
}
