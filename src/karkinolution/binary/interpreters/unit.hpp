#pragma once

#include "karkinolution/binary/fields.hpp"
#include "karkinolution/binary/serializer.hpp"
#include "karkinolution/math/units.hpp"
#include "karkinolution/organism/stats.hpp"

#include <array>
#include <cstddef>
#include <karkinolution/binary/interpreters/stats.hpp>
#include <vector>

namespace Karkinolution {
	namespace Binary {
		namespace Interpreters {
			namespace SRI {
				namespace Units {

					using StandardPhysicBytes = Serializer::Core::EightBytes;
					// respects the invariant of physics types
					static_assert(sizeof(StandardPhysicBytes) == sizeof(double));

					/*
					 * Size protocol
					 *
					 * [8 bytes - lateral]
					 * [8 bytes - height]
					 * [8 bytes - depth]
					 *
					 */
					namespace Layout {
						using Lateral = Field<0, sizeof(double)>;
						using Height  = Next<Lateral, sizeof(double)>;
						using Depth   = Next<Height, sizeof(double)>;

						inline constexpr std::size_t TOTAL_SIZE = Depth::END;
					} // namespace Layout

					namespace BytesArrays {
						using Size = std::array<std::byte, Layout::TOTAL_SIZE>;
					} // namespace BytesArrays

					StandardPhysicBytes serialize_volume(Volume volume);
					StandardPhysicBytes serialize_mass(PhysicsStats::Mass mass);
					StandardPhysicBytes serialize_density(PhysicsStats::Density density);
					StandardPhysicBytes serialize_meter(Meter meter);

					StandardPhysicBytes serialize_lateral(Lateral lateral);
					StandardPhysicBytes serialize_height(Height height);
					StandardPhysicBytes serialize_depth(Depth depth);

					BytesArrays::Size serialize_size(const Size &size);

					StatsSRI::BytesArray::Stat
					serialize_shared_volume(const PhysicsStats::SharedVolume &shared_volume);
					StatsSRI::BytesArray::Stat serialize_efficiency(const Efficiency &efficiency);
					StatsSRI::BytesArray::Stat serialize_quality(const Quality &quality);

				} // namespace Units
			} // namespace SRI

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

namespace UnitsSRI = Karkinolution::Binary::Interpreters::SRI::Units;
namespace UnitsDSI = Karkinolution::Binary::Interpreters::DSI::Units;
namespace Units    = UnitsSRI;
