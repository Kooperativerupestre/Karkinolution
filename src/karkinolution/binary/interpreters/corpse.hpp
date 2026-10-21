#pragma once

#include "karkinolution/binary/fields.hpp"
#include "karkinolution/binary/interpreters/identity.hpp"
#include "karkinolution/binary/message_type_size.hpp"
#include "karkinolution/core/id.hpp"
#include "karkinolution/math/physic/vec/model.hpp"
#include "karkinolution/math/units.hpp"
#include "karkinolution/organism/stats.hpp"

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
					 * [28 -> 35 - position.z]
					 * # end position
					 *
					 * # begin size (24 bytes)
					 * [36 -> 43 - size.lateral]
					 * [44 -> 51 - size.height]
					 * [52 -> 59 - size.depth]
					 * # end size
					 */

					namespace Layout {
						using Id        = Field<0, sizeof(BaseIdType)>;
						using RawMeat   = Next<Id, 4>;
						using PositionX = Next<RawMeat, VecSRI::Layout::AXIS_SIZE>;
						using PositionY = Next<PositionX, VecSRI::Layout::AXIS_SIZE>;
						using PositionZ = Next<PositionY, VecSRI::Layout::AXIS_SIZE>;
						using Lateral   = Next<PositionZ, UnitsSRI::Layout::Lateral::SIZE>;
						using Height    = Next<Lateral, Units::Layout::Height::SIZE>;
						using Depth     = Next<Height, Units::Layout::Depth::SIZE>;

						inline constexpr std::size_t TOTAL_SIZE = Depth::END;
					} // namespace Layout

					namespace BytesArrays {
						using Corpse = std::array<std::byte, Layout::TOTAL_SIZE>;
					} // namespace BytesArrays

					IdBytes                     serialize_id(BaseIdType id);
					Serializer::Core::FourBytes serialize_raw_meat(const RawMeat &raw_meat);
					VecSRI::BytesArrays::Vec    serialize_position(const Vec3 &position);
					Units::BytesArrays::Size    serialize_size(const Size &size);

					BytesArrays::Corpse serialize_corpse(const class Corpse &corpse);

					namespace Request {
						std::array<std::byte, 8> make_see_corpse(BaseIdType id);
					} // namespace Request
				} // namespace Corpse
			} // namespace SRI

			namespace DSI {
				namespace Corpse {
					namespace Models {

						struct Corpse {
								BaseIdType                   id;
								OrganismStats::Body::RawMeat raw_meat;
								Vec3                         position;
								Size                         size;
						};
					} // namespace Models

					BaseIdType get_id(const std::vector<std::byte> &payload, std::size_t offset);
					OrganismStats::Body::RawMeat get_raw_meat(const std::vector<std::byte> &payload,
															  std::size_t                   offset);
					Vec3 get_position(const std::vector<std::byte> &payload, std::size_t offset);
					Size get_size(const std::vector<std::byte> &payload, std::size_t offset);
					Models::Corpse get_corpse(const std::vector<std::byte> &payload,
											  std::size_t                   offset);

					namespace Request {
						BaseIdType interpret_like_see_corpse(const std::vector<std::byte> &payload);
					} // namespace Request
				} // namespace Corpse
			} // namespace DSI
		} // namespace Interpreters
	} // namespace Binary
} // namespace Karkinolution

namespace CorpseSRI = Karkinolution::Binary::Interpreters::SRI::Corpse;
namespace CorpseDSI = Karkinolution::Binary::Interpreters::DSI::Corpse;
