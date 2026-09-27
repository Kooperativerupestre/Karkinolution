#pragma once
#include "karkinolution/organism/stats.hpp"

#include <karkinolution/binary/serialization/interpreters/math/stats/stats.hpp>
#include <karkinolution/binary/serialization/serializer.hpp>
#include <karkinolution/math/units.hpp>

namespace PhysicsUnitsSRI {
	inline constexpr std::size_t TO_GET_LATERAL_OFFSET = 0;
	inline constexpr std::size_t TO_GET_HEIGHT_OFFSET  = TO_GET_LATERAL_OFFSET + sizeof(double);
	inline constexpr std::size_t TO_GET_DEPTH_OFFSET   = TO_GET_HEIGHT_OFFSET + sizeof(double);
	using SizeBytes                                    = std::array<std::byte, sizeof(double) * 3>;


	Serializer::Types::DoubleBytes serialize_volume(Volume volume);
	Serializer::Types::DoubleBytes serialize_mass(PhysicsStats::Mass mass);
	Serializer::Types::DoubleBytes serialize_density(PhysicsStats::Density density);
	Serializer::Types::DoubleBytes serialize_meter(Meter meter);

	Serializer::Types::DoubleBytes serialize_lateral(Lateral lateral);
	Serializer::Types::DoubleBytes serialize_height(Height height);
	Serializer::Types::DoubleBytes serialize_depth(Depth depth);


	/*
	 * Size protocol
	 *
	 * [8 bytes - lateral]
	 * [8 bytes - height]
	 * [8 bytes - depth]
	 *
	 */


	SizeBytes serialize_size(const Size &size);

	StatsSRI::StatBytes serialize_shared_volume(const PhysicsStats::SharedVolume &shared_volume);
	StatsSRI::StatBytes serialize_efficiency(const Efficiency &efficiency);
	StatsSRI::StatBytes serialize_quality(const Quality &quality);


} // namespace PhysicsUnitsSRI