#include "shared/binary/interpreters/unit.hpp"

#include "shared/binary/byte_utils.hpp"
#include "shared/binary/deserializer.hpp"
#include "shared/binary/serializer.hpp"

#include <shared/binary/interpreters/stats.hpp>

// SRI implementations
UnitsSRI::StandardPhysicBytes UnitsSRI::serialize_area(KShared::Math::Area area) {
	return KShared::Binary::Serializer::convert_double(area.value);
}

UnitsSRI::StandardPhysicBytes UnitsSRI::serialize_mass(KShared::Math::Units::Mass mass) {
	return KShared::Binary::Serializer::convert_double(mass.value);
}

UnitsSRI::StandardPhysicBytes UnitsSRI::serialize_density(KShared::Math::Units::Density density) {
	return KShared::Binary::Serializer::convert_double(density.value);
}

UnitsSRI::StandardPhysicBytes UnitsSRI::serialize_meter(KShared::Math::Units::Meter meter) {
	return KShared::Binary::Serializer::convert_double(meter.value);
}

UnitsSRI::StandardPhysicBytes UnitsSRI::serialize_width(KShared::Math::Units::Width width) {
	return KShared::Binary::Serializer::convert_double(width.value);
}

UnitsSRI::StandardPhysicBytes UnitsSRI::serialize_height(KShared::Math::Units::Height height) {
	return KShared::Binary::Serializer::convert_double(height.value);
}

UnitsSRI::BytesArrays::Size UnitsSRI::serialize_size(const KShared::Math::Units::Size &size) {
	UnitsSRI::BytesArrays::Size bytes;

	const auto width_serialized = UnitsSRI::serialize_width(size.width);
	KShared::Binary::Utils::append_bytes(bytes, width_serialized, Layout::Width::OFFSET);

	const auto height_serialized = UnitsSRI::serialize_height(size.height);
	KShared::Binary::Utils::append_bytes(bytes, height_serialized, Layout::Height::OFFSET);

	return bytes;
}

StatsSRI::BytesArray::Stat UnitsSRI::serialize_shared_area(const KShared::Math::SharedArea &shared_area) {
	return StatsSRI::serialize_normalized_value(shared_area);
}

StatsSRI::BytesArray::Stat UnitsSRI::serialize_efficiency(const KShared::Math::Efficiency &efficiency) {
	return StatsSRI::serialize_normalized_value(efficiency);
}

StatsSRI::BytesArray::Stat UnitsSRI::serialize_quality(const KShared::Math::Quality &quality) {
	return StatsSRI::serialize_normalized_value(quality);
}

// DSI implementations
KShared::Math::Area UnitsDSI::deserialize_area(const std::vector<std::byte> &payload, std::size_t offset) {
	return KShared::Math::Area(KShared::Binary::Deserializer::read_double(payload, offset));
}

KShared::Math::Units::Mass UnitsDSI::deserialize_mass(const std::vector<std::byte> &payload, std::size_t offset) {
	return KShared::Math::Units::Mass(KShared::Binary::Deserializer::read_double(payload, offset));
}

KShared::Math::Units::Density UnitsDSI::deserialize_density(const std::vector<std::byte> &payload, std::size_t offset) {
	return KShared::Math::Units::Density(KShared::Binary::Deserializer::read_double(payload, offset));
}

KShared::Math::Units::Meter UnitsDSI::deserialize_meter(const std::vector<std::byte> &payload, std::size_t offset) {
	return KShared::Math::Units::Meter(KShared::Binary::Deserializer::read_double(payload, offset));
}

KShared::Math::Units::Width UnitsDSI::deserialize_width(const std::vector<std::byte> &payload, std::size_t offset) {
	return KShared::Math::Units::Width(
		KShared::Binary::Deserializer::read_double(payload, offset + UnitsSRI::Layout::Width::OFFSET));
}

KShared::Math::Units::Height UnitsDSI::deserialize_height(const std::vector<std::byte> &payload, std::size_t offset) {
	return KShared::Math::Units::Height(
		KShared::Binary::Deserializer::read_double(payload, offset + UnitsSRI::Layout::Height::OFFSET));
}

KShared::Math::Units::Size UnitsDSI::deserialize_size(const std::vector<std::byte> &payload, std::size_t offset) {
	const auto width  = deserialize_width(payload, offset);
	const auto height = deserialize_height(payload, offset);

	return KShared::Math::Units::Size{.width = width, .height = height};
}

KShared::Math::SharedArea UnitsDSI::deserialize_shared_area(const std::vector<std::byte> &payload, std::size_t offset) {
	return KShared::Math::SharedArea(StatsDSI::get_value<double>(payload, offset));
}

KShared::Math::Efficiency UnitsDSI::deserialize_efficiency(const std::vector<std::byte> &payload, std::size_t offset) {
	return KShared::Math::Efficiency(StatsDSI::get_value<float>(payload, offset));
}

KShared::Math::Quality UnitsDSI::deserialize_quality(const std::vector<std::byte> &payload, std::size_t offset) {
	return KShared::Math::Quality(StatsDSI::get_value<float>(payload, offset));
}
