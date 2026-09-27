#pragma once

#include <cstddef>
#include <karkinolution/math/units.hpp>
#include <karkinolution/organism/stats.hpp>
#include <vector>

namespace PhysicsUnitsDSI {
	Volume deserialize_volume(const std::vector<std::byte> &payload, std::size_t offset = 0);
	PhysicsStats::Mass    deserialize_mass(const std::vector<std::byte> &payload,
										   std::size_t                   offset = 0);
	PhysicsStats::Density deserialize_density(const std::vector<std::byte> &payload,
											  std::size_t                   offset = 0);
	Meter deserialize_meter(const std::vector<std::byte> &payload, std::size_t offset = 0);

	Lateral deserialize_lateral(const std::vector<std::byte> &payload, std::size_t offset = 0);
	Height  deserialize_height(const std::vector<std::byte> &payload, std::size_t offset = 0);
	Depth   deserialize_depth(const std::vector<std::byte> &payload, std::size_t offset = 0);

	Size deserialize_size(const std::vector<std::byte> &payload, std::size_t offset = 0);

	PhysicsStats::SharedVolume deserialize_shared_volume(const std::vector<std::byte> &payload,
														 std::size_t                   offset = 0);
	Efficiency                 deserialize_efficiency(const std::vector<std::byte> &payload,
													  std::size_t                   offset = 0);
	Quality deserialize_quality(const std::vector<std::byte> &payload, std::size_t offset = 0);
} // namespace PhysicsUnitsDSI
