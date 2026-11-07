#pragma once

#include "karkinolution/binary/fields.hpp"
#include "karkinolution/binary/interpreters/identity.hpp"
#include "karkinolution/binary/interpreters/models/corpse.hpp"
#include "shared/core/id.hpp"
#include "shared/math/physic/vec/model.hpp"
#include "shared/math/units.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <karkinolution/binary/core_aliases.hpp>
#include <karkinolution/binary/interpreters/unit.hpp>
#include <karkinolution/binary/interpreters/vec.hpp>
#include <karkinolution/binary/serializer.hpp>
#include <karkinolution/organism/entities/corpse/corpse.hpp>
#include <vector>

namespace Karkinolution {
	namespace Binary {
		namespace Interpreters {
			namespace SRI {
				namespace Corpse {
					/* CORPSE PROTOCOL
					 * [0 -> 7 - id (8 bytes)]
					 * [8 -> 11 - raw_meat (4 bytes)]
					 *
					 * # begin position (24 bytes)
					 * [12 -> 19 - position.x]
					 * [20 -> 27 - position.y]
					 * # end position
					 *
					 * # begin size (24 bytes)
					 * [36 -> 43 - size.width]
					 * [44 -> 51 - size.height]
					 * # end size
					 */

					namespace Layout {
						using Id        = Field<0, sizeof(KShared::Core::Id::BaseIdType)>;
						using RawMeat   = Next<Id, 4>;
						using PositionX = Next<RawMeat, VecSRI::Layout::AXIS_SIZE>;
						using PositionY = Next<PositionX, VecSRI::Layout::AXIS_SIZE>;
						using Width     = Next<PositionY, UnitsSRI::Layout::Width::SIZE>;
						using Height    = Next<Width, Units::Layout::Height::SIZE>;

						inline constexpr std::size_t TOTAL_SIZE = Height::END;
					} // namespace Layout

					namespace BytesArrays {
						using Corpse = std::array<std::byte, Layout::TOTAL_SIZE>;
					} // namespace BytesArrays

					IdBytes serialize_id(KShared::Core::Id::BaseIdType id);
					Karkinolution::Binary::Serializer::Core::FourBytes
											 serialize_raw_meat(const Karkinolution::Organisms::RawMeat &raw_meat);
					VecSRI::BytesArrays::Vec serialize_position(const KShared::Math::Vec::Vec2 &position);
					Units::BytesArrays::Size serialize_size(const KShared::Math::Units::Size &size);

					BytesArrays::Corpse
					serialize_corpse(const Karkinolution::Organisms::Entities::Corpse::Corpse &corpse);

					namespace Request {
						std::array<std::byte, 8> make_see_corpse(KShared::Core::Id::BaseIdType id);
					} // namespace Request
				} // namespace Corpse
			} // namespace SRI

			namespace DSI {
				namespace Corpse {
					KShared::Core::Id::BaseIdType  get_id(const std::vector<std::byte> &payload,
																std::size_t                   offset);
					Karkinolution::Organisms::RawMeat    get_raw_meat(const std::vector<std::byte> &payload,
																	  std::size_t                   offset);
					KShared::Math::Vec::Vec2       get_position(const std::vector<std::byte> &payload,
																	  std::size_t                   offset);
					KShared::Math::Units::Size     get_size(const std::vector<std::byte> &payload,
																  std::size_t                   offset);
					Interpreters::Models::Corpse::Corpse get_corpse(const std::vector<std::byte> &payload,
																	std::size_t                   offset);

					namespace Request {
						KShared::Core::Id::BaseIdType
						interpret_like_see_corpse(const std::vector<std::byte> &payload);
					} // namespace Request
				} // namespace Corpse
			} // namespace DSI
		} // namespace Interpreters
	} // namespace Binary
} // namespace Karkinolution

namespace CorpseSRI = Karkinolution::Binary::Interpreters::SRI::Corpse;
namespace CorpseDSI = Karkinolution::Binary::Interpreters::DSI::Corpse;
