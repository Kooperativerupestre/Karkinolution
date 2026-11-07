#pragma once

#include "karkinolution/binary/fields.hpp"
#include "karkinolution/binary/serializer.hpp"
#include "shared/math/units.hpp"

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
					 * [8 bytes - width]
					 * [8 bytes - height]
					 *
					 */
					namespace Layout {
						using Width  = Field<0, sizeof(double)>;
						using Height = Next<Width, sizeof(double)>;

						inline constexpr std::size_t TOTAL_SIZE = Height::END;
					} // namespace Layout

					namespace BytesArrays {
						using Size = std::array<std::byte, Layout::TOTAL_SIZE>;
					} // namespace BytesArrays

					StandardPhysicBytes serialize_area(KShared::Math::Area area);
					StandardPhysicBytes serialize_mass(KShared::Math::Units::Mass mass);
					StandardPhysicBytes serialize_density(KShared::Math::Units::Density density);
					StandardPhysicBytes serialize_meter(KShared::Math::Units::Meter meter);

					StandardPhysicBytes serialize_width(KShared::Math::Units::Width width);
					StandardPhysicBytes serialize_height(KShared::Math::Units::Height height);

					BytesArrays::Size serialize_size(const KShared::Math::Units::Size &size);

					StatsSRI::BytesArray::Stat serialize_shared_area(const KShared::Math::SharedArea &shared_area);
					StatsSRI::BytesArray::Stat serialize_efficiency(const KShared::Math::Efficiency &efficiency);
					StatsSRI::BytesArray::Stat serialize_quality(const KShared::Math::Quality &quality);

				} // namespace Units
			} // namespace SRI

			namespace DSI {
				namespace Units {
					KShared::Math::Area deserialize_area(const std::vector<std::byte> &payload, std::size_t offset);
					KShared::Math::Units::Mass    deserialize_mass(const std::vector<std::byte> &payload,
																   std::size_t                   offset);
					KShared::Math::Units::Density deserialize_density(const std::vector<std::byte> &payload,
																	  std::size_t                   offset);
					KShared::Math::Units::Meter   deserialize_meter(const std::vector<std::byte> &payload,
																	std::size_t                   offset);

					KShared::Math::Units::Width  deserialize_width(const std::vector<std::byte> &payload,
																   std::size_t                   offset);
					KShared::Math::Units::Height deserialize_height(const std::vector<std::byte> &payload,
																	std::size_t                   offset);

					KShared::Math::Units::Size deserialize_size(const std::vector<std::byte> &payload,
																std::size_t                   offset);

					KShared::Math::SharedArea deserialize_shared_area(const std::vector<std::byte> &payload,
																	  std::size_t                   offset);
					KShared::Math::Efficiency deserialize_efficiency(const std::vector<std::byte> &payload,
																	 std::size_t                   offset);
					KShared::Math::Quality    deserialize_quality(const std::vector<std::byte> &payload,
																  std::size_t                   offset);
				} // namespace Units
			} // namespace DSI
		} // namespace Interpreters
	} // namespace Binary
} // namespace Karkinolution

namespace UnitsSRI = Karkinolution::Binary::Interpreters::SRI::Units;
namespace UnitsDSI = Karkinolution::Binary::Interpreters::DSI::Units;
namespace Units    = UnitsSRI;
