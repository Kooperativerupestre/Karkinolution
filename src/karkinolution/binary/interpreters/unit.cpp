#include "karkinolution/binary/interpreters/unit.hpp"

#include "karkinolution/binary/byte_utils.hpp"
#include "karkinolution/binary/deserializer.hpp"
#include "karkinolution/binary/serializer.hpp"

#include <karkinolution/binary/interpreters/stats.hpp>

// SRI implementations
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
	Karkinolution::Binary::Utils::append_bytes(bytes, lateral_serialized, Layout::Lateral::OFFSET);

	const auto height_serialized = UnitsSRI::serialize_height(size.height);
	Karkinolution::Binary::Utils::append_bytes(bytes, height_serialized, Layout::Height::OFFSET);

	const auto depth_serialized = UnitsSRI::serialize_depth(size.depth);
	Karkinolution::Binary::Utils::append_bytes(bytes, depth_serialized, Layout::Depth::OFFSET);

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

// DSI implementations
Volume UnitsDSI::deserialize_volume(const std::vector<std::byte> &payload, std::size_t offset) {
	return Volume(Deserializer::read_double(payload, offset));
}

PhysicsStats::Mass UnitsDSI::deserialize_mass(const std::vector<std::byte> &payload,
											  std::size_t                   offset) {
	return PhysicsStats::Mass(Deserializer::read_double(payload, offset));
}

PhysicsStats::Density UnitsDSI::deserialize_density(const std::vector<std::byte> &payload,
													std::size_t                   offset) {
	return PhysicsStats::Density(Deserializer::read_double(payload, offset));
}

Meter UnitsDSI::deserialize_meter(const std::vector<std::byte> &payload, std::size_t offset) {
	return Meter(Deserializer::read_double(payload, offset));
}

Lateral UnitsDSI::deserialize_lateral(const std::vector<std::byte> &payload, std::size_t offset) {
	return Lateral(Deserializer::read_double(payload, offset + UnitsSRI::Layout::Lateral::OFFSET));
}

Height UnitsDSI::deserialize_height(const std::vector<std::byte> &payload, std::size_t offset) {
	return Height(Deserializer::read_double(payload, offset + UnitsSRI::Layout::Height::OFFSET));
}

Depth UnitsDSI::deserialize_depth(const std::vector<std::byte> &payload, std::size_t offset) {
	return Depth(Deserializer::read_double(payload, offset + UnitsSRI::Layout::Depth::OFFSET));
}

Size UnitsDSI::deserialize_size(const std::vector<std::byte> &payload, std::size_t offset) {
	const auto lateral = deserialize_lateral(payload, offset);
	const auto height  = deserialize_height(payload, offset);
	const auto depth   = deserialize_depth(payload, offset);

	return Size{.lateral = lateral, .height = height, .depth = depth};
}

PhysicsStats::SharedVolume
UnitsDSI::deserialize_shared_volume(const std::vector<std::byte> &payload, std::size_t offset) {
	return PhysicsStats::SharedVolume(StatsDSI::get_value<double>(payload, offset));
}

Efficiency UnitsDSI::deserialize_efficiency(const std::vector<std::byte> &payload,
											std::size_t                   offset) {
	return Efficiency(StatsDSI::get_value<float>(payload, offset));
}

Quality UnitsDSI::deserialize_quality(const std::vector<std::byte> &payload, std::size_t offset) {
	return Quality(StatsDSI::get_value<float>(payload, offset));
}
