#pragma once

#include "karkinolution/binary/fields.hpp"
#include "karkinolution/binary/serializer.hpp"
#include "karkinolution/math/units.hpp"

#include <array>
#include <cstddef>
#include <karkinolution/binary/interpreters/stats.hpp>
#include <vector>

namespace Karkinolution {
	namespace Binary {
		namespace Interpreters {
			namespace SRI {
				namespace Units {

					using StandardPhysicBytes = Karkinolution::Binary::Serializer::Core::EightBytes;
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

					StandardPhysicBytes serialize_volume(Karkinolution::Math::Volume volume);
					StandardPhysicBytes
					serialize_mass(Karkinolution::Math::Units::Mass mass);
					StandardPhysicBytes
					serialize_density(Karkinolution::Math::Units::Density density);
					StandardPhysicBytes
					serialize_meter(Karkinolution::Math::Units::Meter meter);

					StandardPhysicBytes
					serialize_lateral(Karkinolution::Math::Units::Lateral lateral);
					StandardPhysicBytes serialize_height(Karkinolution::Math::Units::Height height);
					StandardPhysicBytes serialize_depth(Karkinolution::Math::Units::Depth depth);

					BytesArrays::Size serialize_size(const Karkinolution::Math::Units::Size &size);

					StatsSRI::BytesArray::Stat
					serialize_shared_volume(const Karkinolution::Math::SharedVolume &shared_volume);
					StatsSRI::BytesArray::Stat
					serialize_efficiency(const Karkinolution::Math::Efficiency &efficiency);
					StatsSRI::BytesArray::Stat
					serialize_quality(const Karkinolution::Math::Quality &quality);

				} // namespace Units
			} // namespace SRI

			namespace DSI {
				namespace Units {
					Karkinolution::Math::Volume
					deserialize_volume(const std::vector<std::byte> &payload, std::size_t offset);
					Karkinolution::Math::Units::Mass
					deserialize_mass(const std::vector<std::byte> &payload, std::size_t offset);
					Karkinolution::Math::Units::Density
					deserialize_density(const std::vector<std::byte> &payload, std::size_t offset);
					Karkinolution::Math::Units::Meter
					deserialize_meter(const std::vector<std::byte> &payload, std::size_t offset);

					Karkinolution::Math::Units::Lateral
					deserialize_lateral(const std::vector<std::byte> &payload, std::size_t offset);
					Karkinolution::Math::Units::Height
					deserialize_height(const std::vector<std::byte> &payload, std::size_t offset);
					Karkinolution::Math::Units::Depth
					deserialize_depth(const std::vector<std::byte> &payload, std::size_t offset);

					Karkinolution::Math::Units::Size
					deserialize_size(const std::vector<std::byte> &payload, std::size_t offset);

					Karkinolution::Math::SharedVolume
					deserialize_shared_volume(const std::vector<std::byte> &payload,
											  std::size_t offset);
					Karkinolution::Math::Efficiency
					deserialize_efficiency(const std::vector<std::byte> &payload,
										   std::size_t offset);
					Karkinolution::Math::Quality
					deserialize_quality(const std::vector<std::byte> &payload, std::size_t offset);
				} // namespace Units
			} // namespace DSI
		} // namespace Interpreters
	} // namespace Binary
} // namespace Karkinolution

namespace UnitsSRI = Karkinolution::Binary::Interpreters::SRI::Units;
namespace UnitsDSI = Karkinolution::Binary::Interpreters::DSI::Units;
namespace Units    = UnitsSRI;
