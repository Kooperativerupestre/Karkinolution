#include "karkinolution/binary/deserialization/interpreters/math/unit/unit.hpp"

#include <karkinolution/binary/deserialization/deserializer.hpp>
#include <karkinolution/binary/deserialization/interpreters/math/stats/alias.hpp>
#include <karkinolution/binary/deserialization/interpreters/math/stats/stats.hpp>
#include <karkinolution/binary/deserialization/interpreters/math/unit/alias.hpp>
#include <karkinolution/binary/serialization/interpreters/math/stats/stats.hpp>
#include <karkinolution/binary/serialization/interpreters/math/unit/alias.hpp>
#include <karkinolution/binary/serialization/interpreters/math/unit/unit.hpp>

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
