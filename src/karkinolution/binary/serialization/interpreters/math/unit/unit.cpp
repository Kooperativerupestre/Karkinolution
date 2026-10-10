#include "karkinolution/binary/serialization/interpreters/math/stats/stats.hpp"

#include <karkinolution/binary/serialization/interpreters/math/unit/alias.hpp>
#include <karkinolution/binary/serialization/interpreters/math/unit/unit.hpp>

UnitsSRI::StandardPhysicBytes UnitsSRI::serialize_volume(Volume volume) {
	return Serializer::convert_double(volume.value);
}

UnitsSRI::StandardPhysicBytes UnitsSRI::serialize_mass(PhysicsStats::Mass mass) {
	return Serializer::convert_double(mass.value);
}

UnitsSRI::StandardPhysicBytes UnitsSRI::serialize_density(PhysicsStats::Density density) {
	return Serializer::convert_double(density.value);
}

UnitsSRI::StandardPhysicBytes UnitsSRI::serialize_meter(PhysicsStats::Meter meter) {
	return Serializer::convert_double(meter.value);
}

UnitsSRI::StandardPhysicBytes UnitsSRI::serialize_lateral(Lateral lateral) {
	return Serializer::convert_double(lateral.value);
}

UnitsSRI::StandardPhysicBytes UnitsSRI::serialize_height(Height height) {
	return Serializer::convert_double(height.value);
}

UnitsSRI::StandardPhysicBytes UnitsSRI::serialize_depth(Depth depth) {
	return Serializer::convert_double(depth.value);
}

UnitsSRI::BytesArrays::Size UnitsSRI::serialize_size(const Size &size) {
	UnitsSRI::BytesArrays::Size bytes;

	const auto lateral_serialized = UnitsSRI::serialize_lateral(size.lateral);
	Deserializer::append_bytes(bytes, lateral_serialized, Layout::Lateral::OFFSET);

	const auto height_serialized = UnitsSRI::serialize_height(size.height);
	Deserializer::append_bytes(bytes, height_serialized, Layout::Height::OFFSET);

	const auto depth_serialized = UnitsSRI::serialize_depth(size.depth);
	Deserializer::append_bytes(bytes, depth_serialized, Layout::Depth::OFFSET);

	return bytes;
}

StatsSRI::BytesArray::Stat
UnitsSRI::serialize_shared_volume(const PhysicsStats::SharedVolume &shared_volume) {
	return StatsSRI::serialize_normalized_value(shared_volume);
}

StatsSRI::BytesArray::Stat UnitsSRI::serialize_efficiency(const Efficiency &efficiency) {
	return StatsSRI::serialize_normalized_value(efficiency);
}

StatsSRI::BytesArray::Stat UnitsSRI::serialize_quality(const Quality &quality) {
	return StatsSRI::serialize_normalized_value(quality);
}