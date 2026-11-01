#include "karkinolution/binary/interpreters/unit.hpp"

#include "karkinolution/binary/byte_utils.hpp"
#include "karkinolution/binary/deserializer.hpp"
#include "karkinolution/binary/serializer.hpp"

#include <karkinolution/binary/interpreters/stats.hpp>

// SRI implementations
UnitsSRI::StandardPhysicBytes
UnitsSRI::serialize_volume(Karkinolution::Math::Volume volume) {
	return Karkinolution::Binary::Serializer::convert_double(volume.value);
}

UnitsSRI::StandardPhysicBytes
UnitsSRI::serialize_mass(Karkinolution::Math::Units::Mass mass) {
	return Karkinolution::Binary::Serializer::convert_double(mass.value);
}

UnitsSRI::StandardPhysicBytes
UnitsSRI::serialize_density(Karkinolution::Math::Units::Density density) {
	return Karkinolution::Binary::Serializer::convert_double(density.value);
}

UnitsSRI::StandardPhysicBytes
UnitsSRI::serialize_meter(Karkinolution::Math::Units::Meter meter) {
	return Karkinolution::Binary::Serializer::convert_double(meter.value);
}

UnitsSRI::StandardPhysicBytes
UnitsSRI::serialize_lateral(Karkinolution::Math::Units::Lateral lateral) {
	return Karkinolution::Binary::Serializer::convert_double(lateral.value);
}

UnitsSRI::StandardPhysicBytes
UnitsSRI::serialize_height(Karkinolution::Math::Units::Height height) {
	return Karkinolution::Binary::Serializer::convert_double(height.value);
}

UnitsSRI::StandardPhysicBytes
UnitsSRI::serialize_depth(Karkinolution::Math::Units::Depth depth) {
	return Karkinolution::Binary::Serializer::convert_double(depth.value);
}

UnitsSRI::BytesArrays::Size
UnitsSRI::serialize_size(const Karkinolution::Math::Units::Size &size) {
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
UnitsSRI::serialize_shared_volume(const Karkinolution::Math::SharedVolume &shared_volume) {
	return StatsSRI::serialize_normalized_value(shared_volume);
}

StatsSRI::BytesArray::Stat
UnitsSRI::serialize_efficiency(const Karkinolution::Math::Efficiency &efficiency) {
	return StatsSRI::serialize_normalized_value(efficiency);
}

StatsSRI::BytesArray::Stat
UnitsSRI::serialize_quality(const Karkinolution::Math::Quality &quality) {
	return StatsSRI::serialize_normalized_value(quality);
}

// DSI implementations
Karkinolution::Math::Volume
UnitsDSI::deserialize_volume(const std::vector<std::byte> &payload, std::size_t offset) {
	return Karkinolution::Math::Volume(Karkinolution::Binary::Deserializer::read_double(payload, offset));
}

Karkinolution::Math::Units::Mass
UnitsDSI::deserialize_mass(const std::vector<std::byte> &payload, std::size_t offset) {
	return Karkinolution::Math::Units::Mass(Karkinolution::Binary::Deserializer::read_double(payload, offset));
}

Karkinolution::Math::Units::Density
UnitsDSI::deserialize_density(const std::vector<std::byte> &payload, std::size_t offset) {
	return Karkinolution::Math::Units::Density(Karkinolution::Binary::Deserializer::read_double(payload, offset));
}

Karkinolution::Math::Units::Meter
UnitsDSI::deserialize_meter(const std::vector<std::byte> &payload, std::size_t offset) {
	return Karkinolution::Math::Units::Meter(Karkinolution::Binary::Deserializer::read_double(payload, offset));
}

Karkinolution::Math::Units::Lateral
UnitsDSI::deserialize_lateral(const std::vector<std::byte> &payload, std::size_t offset) {
	return Karkinolution::Math::Units::Lateral(
		Karkinolution::Binary::Deserializer::read_double(payload, offset + UnitsSRI::Layout::Lateral::OFFSET));
}

Karkinolution::Math::Units::Height
UnitsDSI::deserialize_height(const std::vector<std::byte> &payload, std::size_t offset) {
	return Karkinolution::Math::Units::Height(
		Karkinolution::Binary::Deserializer::read_double(payload, offset + UnitsSRI::Layout::Height::OFFSET));
}

Karkinolution::Math::Units::Depth
UnitsDSI::deserialize_depth(const std::vector<std::byte> &payload, std::size_t offset) {
	return Karkinolution::Math::Units::Depth(
		Karkinolution::Binary::Deserializer::read_double(payload, offset + UnitsSRI::Layout::Depth::OFFSET));
}

Karkinolution::Math::Units::Size
UnitsDSI::deserialize_size(const std::vector<std::byte> &payload, std::size_t offset) {
	const auto lateral = deserialize_lateral(payload, offset);
	const auto height  = deserialize_height(payload, offset);
	const auto depth   = deserialize_depth(payload, offset);

	return Karkinolution::Math::Units::Size{.lateral = lateral, .height = height, .depth = depth};
}

Karkinolution::Math::SharedVolume
UnitsDSI::deserialize_shared_volume(const std::vector<std::byte> &payload, std::size_t offset) {
	return Karkinolution::Math::SharedVolume(StatsDSI::get_value<double>(payload, offset));
}

Karkinolution::Math::Efficiency
UnitsDSI::deserialize_efficiency(const std::vector<std::byte> &payload, std::size_t offset) {
	return Karkinolution::Math::Efficiency(StatsDSI::get_value<float>(payload, offset));
}

Karkinolution::Math::Quality
UnitsDSI::deserialize_quality(const std::vector<std::byte> &payload, std::size_t offset) {
	return Karkinolution::Math::Quality(StatsDSI::get_value<float>(payload, offset));
}
