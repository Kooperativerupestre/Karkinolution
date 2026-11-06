#include "karkinolution/binary/interpreters/unit.hpp"

#include "karkinolution/binary/byte_utils.hpp"
#include "karkinolution/binary/deserializer.hpp"
#include "karkinolution/binary/serializer.hpp"

#include <karkinolution/binary/interpreters/stats.hpp>

// SRI implementations
UnitsSRI::StandardPhysicBytes UnitsSRI::serialize_area(Karkinolution::Math::Area area) {
	return Karkinolution::Binary::Serializer::convert_double(area.value);
}

UnitsSRI::StandardPhysicBytes UnitsSRI::serialize_mass(Karkinolution::Math::Units::Mass mass) {
	return Karkinolution::Binary::Serializer::convert_double(mass.value);
}

UnitsSRI::StandardPhysicBytes UnitsSRI::serialize_density(Karkinolution::Math::Units::Density density) {
	return Karkinolution::Binary::Serializer::convert_double(density.value);
}

UnitsSRI::StandardPhysicBytes UnitsSRI::serialize_meter(Karkinolution::Math::Units::Meter meter) {
	return Karkinolution::Binary::Serializer::convert_double(meter.value);
}

UnitsSRI::StandardPhysicBytes UnitsSRI::serialize_width(Karkinolution::Math::Units::Width width) {
	return Karkinolution::Binary::Serializer::convert_double(width.value);
}

UnitsSRI::StandardPhysicBytes UnitsSRI::serialize_height(Karkinolution::Math::Units::Height height) {
	return Karkinolution::Binary::Serializer::convert_double(height.value);
}

UnitsSRI::BytesArrays::Size UnitsSRI::serialize_size(const Karkinolution::Math::Units::Size &size) {
	UnitsSRI::BytesArrays::Size bytes;

	const auto width_serialized = UnitsSRI::serialize_width(size.width);
	Karkinolution::Binary::Utils::append_bytes(bytes, width_serialized, Layout::Width::OFFSET);

	const auto height_serialized = UnitsSRI::serialize_height(size.height);
	Karkinolution::Binary::Utils::append_bytes(bytes, height_serialized, Layout::Height::OFFSET);

	return bytes;
}

StatsSRI::BytesArray::Stat UnitsSRI::serialize_shared_area(const Karkinolution::Math::SharedArea &shared_area) {
	return StatsSRI::serialize_normalized_value(shared_area);
}

StatsSRI::BytesArray::Stat UnitsSRI::serialize_efficiency(const Karkinolution::Math::Efficiency &efficiency) {
	return StatsSRI::serialize_normalized_value(efficiency);
}

StatsSRI::BytesArray::Stat UnitsSRI::serialize_quality(const Karkinolution::Math::Quality &quality) {
	return StatsSRI::serialize_normalized_value(quality);
}

// DSI implementations
Karkinolution::Math::Area UnitsDSI::deserialize_area(const std::vector<std::byte> &payload, std::size_t offset) {
	return Karkinolution::Math::Area(Karkinolution::Binary::Deserializer::read_double(payload, offset));
}

Karkinolution::Math::Units::Mass UnitsDSI::deserialize_mass(const std::vector<std::byte> &payload, std::size_t offset) {
	return Karkinolution::Math::Units::Mass(Karkinolution::Binary::Deserializer::read_double(payload, offset));
}

Karkinolution::Math::Units::Density UnitsDSI::deserialize_density(const std::vector<std::byte> &payload,
																  std::size_t                   offset) {
	return Karkinolution::Math::Units::Density(Karkinolution::Binary::Deserializer::read_double(payload, offset));
}

Karkinolution::Math::Units::Meter UnitsDSI::deserialize_meter(const std::vector<std::byte> &payload,
															  std::size_t                   offset) {
	return Karkinolution::Math::Units::Meter(Karkinolution::Binary::Deserializer::read_double(payload, offset));
}

Karkinolution::Math::Units::Width UnitsDSI::deserialize_width(const std::vector<std::byte> &payload,
															  std::size_t                   offset) {
	return Karkinolution::Math::Units::Width(
		Karkinolution::Binary::Deserializer::read_double(payload, offset + UnitsSRI::Layout::Width::OFFSET));
}

Karkinolution::Math::Units::Height UnitsDSI::deserialize_height(const std::vector<std::byte> &payload,
																std::size_t                   offset) {
	return Karkinolution::Math::Units::Height(
		Karkinolution::Binary::Deserializer::read_double(payload, offset + UnitsSRI::Layout::Height::OFFSET));
}

Karkinolution::Math::Units::Size UnitsDSI::deserialize_size(const std::vector<std::byte> &payload, std::size_t offset) {
	const auto width  = deserialize_width(payload, offset);
	const auto height = deserialize_height(payload, offset);

	return Karkinolution::Math::Units::Size{.width = width, .height = height};
}

Karkinolution::Math::SharedArea UnitsDSI::deserialize_shared_area(const std::vector<std::byte> &payload,
																  std::size_t                   offset) {
	return Karkinolution::Math::SharedArea(StatsDSI::get_value<double>(payload, offset));
}

Karkinolution::Math::Efficiency UnitsDSI::deserialize_efficiency(const std::vector<std::byte> &payload,
																 std::size_t                   offset) {
	return Karkinolution::Math::Efficiency(StatsDSI::get_value<float>(payload, offset));
}

Karkinolution::Math::Quality UnitsDSI::deserialize_quality(const std::vector<std::byte> &payload, std::size_t offset) {
	return Karkinolution::Math::Quality(StatsDSI::get_value<float>(payload, offset));
}
