#include <karkinolution/binary/serialization/interpreters/math/unit/unit.hpp>

Serializer::Types::DoubleBytes PhysicsUnitsSRI::serialize_volume(Volume volume) {
	return Serializer::convert_double(volume.value);
}

Serializer::Types::DoubleBytes PhysicsUnitsSRI::serialize_mass(PhysicsStats::Mass mass) {
	return Serializer::convert_double(mass.value);
}

Serializer::Types::DoubleBytes PhysicsUnitsSRI::serialize_density(PhysicsStats::Density density) {
	return Serializer::convert_double(density.value);
}

Serializer::Types::DoubleBytes PhysicsUnitsSRI::serialize_meter(PhysicsStats::Meter meter) {
	return Serializer::convert_double(meter.value);
}

Serializer::Types::DoubleBytes PhysicsUnitsSRI::serialize_lateral(Lateral lateral) {
	return Serializer::convert_double(lateral.value);
}

Serializer::Types::DoubleBytes PhysicsUnitsSRI::serialize_height(Height height) {
	return Serializer::convert_double(height.value);
}

Serializer::Types::DoubleBytes PhysicsUnitsSRI::serialize_depth(Depth depth) {
	return Serializer::convert_double(depth.value);
}

PhysicsUnitsSRI::SizeBytes PhysicsUnitsSRI::serialize_size(const Size &size) {
	SizeBytes bytes;

	const auto lateral_serialized = PhysicsUnitsSRI::serialize_lateral(size.lateral);
	Deserializer::append_bytes(bytes, lateral_serialized, TO_GET_LATERAL_OFFSET);

	const auto height_serialized = PhysicsUnitsSRI::serialize_height(size.height);
	Deserializer::append_bytes(bytes, height_serialized, TO_GET_HEIGHT_OFFSET);

	const auto depth_serialized = PhysicsUnitsSRI::serialize_depth(size.depth);
	Deserializer::append_bytes(bytes, depth_serialized, TO_GET_DEPTH_OFFSET);

	return bytes;
}

StatsSRI::StatBytes
PhysicsUnitsSRI::serialize_shared_volume(const PhysicsStats::SharedVolume &shared_volume) {
	return StatsSRI::serialize_normalized_value(shared_volume);
}

StatsSRI::StatBytes PhysicsUnitsSRI::serialize_efficiency(const Efficiency &efficiency) {
	return StatsSRI::serialize_normalized_value(efficiency);
}

StatsSRI::StatBytes PhysicsUnitsSRI::serialize_quality(const Quality &quality) {
	return StatsSRI::serialize_normalized_value(quality);
}