#pragma once

#include <cstddef>
#include <karkinolution/math/units.hpp>
#include <karkinolution/organism/stats.hpp>
#include <vector>

namespace Karkinolution {
	namespace Binary {
		namespace Interpreters {
			namespace DSI {
				namespace Units {
					Volume                deserialize_volume(const std::vector<std::byte> &payload,
															 std::size_t                   offset);
					PhysicsStats::Mass    deserialize_mass(const std::vector<std::byte> &payload,
														   std::size_t                   offset);
					PhysicsStats::Density deserialize_density(const std::vector<std::byte> &payload,
															  std::size_t                   offset);
					Meter                 deserialize_meter(const std::vector<std::byte> &payload,
															std::size_t                   offset);

					Lateral deserialize_lateral(const std::vector<std::byte> &payload,
												std::size_t                   offset);
					Height  deserialize_height(const std::vector<std::byte> &payload,
											   std::size_t                   offset);
					Depth   deserialize_depth(const std::vector<std::byte> &payload,
											  std::size_t                   offset);

					Size deserialize_size(const std::vector<std::byte> &payload,
										  std::size_t                   offset);

					PhysicsStats::SharedVolume
							   deserialize_shared_volume(const std::vector<std::byte> &payload,
														 std::size_t                   offset);
					Efficiency deserialize_efficiency(const std::vector<std::byte> &payload,
													  std::size_t                   offset);
					Quality    deserialize_quality(const std::vector<std::byte> &payload,
												   std::size_t                   offset);
				} // namespace Units
			} // namespace DSI
		} // namespace Interpreters
	} // namespace Binary
} // namespace Karkinolution
