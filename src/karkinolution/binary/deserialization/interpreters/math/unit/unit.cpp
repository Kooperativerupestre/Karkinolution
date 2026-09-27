#include "karkinolution/binary/deserialization/interpreters/math/unit/unit.hpp"

#include <karkinolution/binary/deserialization/deserializer.hpp>
#include <karkinolution/binary/deserialization/interpreters/math/stats/stats.hpp>
#include <karkinolution/binary/serialization/interpreters/math/stats/stats.hpp>
#include <karkinolution/binary/serialization/interpreters/math/unit/unit.hpp>

Volume PhysicsUnitsDSI::deserialize_volume(const std::vector<std::byte> &payload,
										   std::size_t                   offset) {
	return Volume(Deserializer::read_double(payload, offset));
}

PhysicsStats::Mass PhysicsUnitsDSI::deserialize_mass(const std::vector<std::byte> &payload,
													 std::size_t                   offset) {
	return PhysicsStats::Mass(Deserializer::read_double(payload, offset));
}

PhysicsStats::Density PhysicsUnitsDSI::deserialize_density(const std::vector<std::byte> &payload,
														   std::size_t                   offset) {
	return PhysicsStats::Density(Deserializer::read_double(payload, offset));
}

Meter PhysicsUnitsDSI::deserialize_meter(const std::vector<std::byte> &payload,
										 std::size_t                   offset) {
	return Meter(Deserializer::read_double(payload, offset));
}

Lateral PhysicsUnitsDSI::deserialize_lateral(const std::vector<std::byte> &payload,
											 std::size_t                   offset) {
	return Lateral(Deserializer::read_double(payload, offset));
}

Height PhysicsUnitsDSI::deserialize_height(const std::vector<std::byte> &payload,
										   std::size_t                   offset) {
	return Height(Deserializer::read_double(payload, offset));
}

Depth PhysicsUnitsDSI::deserialize_depth(const std::vector<std::byte> &payload,
										 std::size_t                   offset) {
	return Depth(Deserializer::read_double(payload, offset));
}

Size PhysicsUnitsDSI::deserialize_size(const std::vector<std::byte> &payload, std::size_t offset) {
	const auto lateral =
		deserialize_lateral(payload, offset + PhysicsUnitsSRI::TO_GET_LATERAL_OFFSET);
	const auto height = deserialize_height(payload, offset + PhysicsUnitsSRI::TO_GET_HEIGHT_OFFSET);
	const auto depth  = deserialize_depth(payload, offset + PhysicsUnitsSRI::TO_GET_DEPTH_OFFSET);

	return Size{.lateral = lateral, .height = height, .depth = depth};
}

PhysicsStats::SharedVolume
PhysicsUnitsDSI::deserialize_shared_volume(const std::vector<std::byte> &payload,
										   std::size_t                   offset) {
	return PhysicsStats::SharedVolume(
		StatsDSI::get_value<double>(payload, offset + StatsSRI::TO_GET_VALUE_OFFSET));
}

Efficiency PhysicsUnitsDSI::deserialize_efficiency(const std::vector<std::byte> &payload,
												   std::size_t                   offset) {
	return Efficiency(StatsDSI::get_value<float>(payload, offset + StatsSRI::TO_GET_VALUE_OFFSET));
}

Quality PhysicsUnitsDSI::deserialize_quality(const std::vector<std::byte> &payload,
											 std::size_t                   offset) {
	return Quality(StatsDSI::get_value<float>(payload, offset + StatsSRI::TO_GET_VALUE_OFFSET));
}
