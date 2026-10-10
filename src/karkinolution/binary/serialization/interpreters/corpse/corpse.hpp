#pragma once

#include "karkinolution/binary/fields.hpp"
#include "karkinolution/binary/message_type_size.hpp"
#include "karkinolution/binary/serialization/interpreters/identity/alias.hpp"
#include "karkinolution/binary/serialization/interpreters/math/unit/alias.hpp"
#include "karkinolution/binary/serialization/interpreters/math/vec/alias.hpp"
#include "karkinolution/binary/serialization/interpreters/math/vec/vec.hpp"
#include "karkinolution/core/id.hpp"
#include "karkinolution/math/units.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <karkinolution/binary/core_aliases.hpp>
#include <karkinolution/binary/serialization/serializer.hpp>
#include <karkinolution/organism/entities/corpse/corpse.hpp>

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
		} // namespace Interpreters
	} // namespace Binary
} // namespace Karkinolution
