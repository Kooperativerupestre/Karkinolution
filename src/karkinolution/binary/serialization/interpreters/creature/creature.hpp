#pragma once


#include "karkinolution/binary/fields.hpp"
#include "karkinolution/binary/message_type_size.hpp"
#include "karkinolution/binary/serialization/interpreters/math/vec/alias.hpp"
#include "karkinolution/binary/serialization/interpreters/math/vec/vec.hpp"
#include "karkinolution/core/id.hpp"

#include <array>
#include <cstdlib>
#include <karkinolution/organism/entities/creature/creature.hpp>

namespace Karkinolution {
	namespace Binary {
		namespace Interpreters {
			namespace SRI {
				namespace Creature {
					/* CREATURE PROTOCOL
					 * [0 - Gender]
					 * [1 - Specie]
					 *
					 * # begin position
					 * [2 -> 9 - x]
					 * [10 -> 17 - y]
					 * [18 -> 25 - z]
					 * # end position
					 * [26 - name size]
					 * [27 -> 90 - name]
					 * [91 -> 99 - id]
					 *
					 */

					namespace Layout {
						inline std::size_t constexpr MAX_NAME_SIZE = 64;

						using Gender    = Field<0, 1>;
						using Specie    = Next<Gender, 1>;
						using PositionX = Next<Specie, VecSRI::Layout::AXIS_SIZE>;
						using PositionY = Next<PositionX, VecSRI::Layout::AXIS_SIZE>;
						using PositionZ = Next<PositionY, VecSRI::Layout::AXIS_SIZE>;
						using NameSize  = Next<PositionZ, 1>;
						using Name      = Next<NameSize, MAX_NAME_SIZE>;
						using Id        = Next<Name, 8>;
						inline constexpr std::size_t TOTAL_SIZE = Id::END;
					} // namespace Layout

					namespace BytesArrays {
						using Creature = std::array<std::byte, Layout::TOTAL_SIZE>;
					} // namespace BytesArrays

					std::byte              serialize_gender(const Gender &gender);
					std::byte              serialize_specie(const CreatureSpecies &specie);
					std::vector<std::byte> serialize_name(const std::string &name);

					BytesArrays::Creature serialize_creature(const class Creature &creature);

					namespace Request {
						std::array<std::byte, 8> make_see_creature(BaseIdType id);
					} // namespace Request
				} // namespace Creature
			} // namespace SRI
		} // namespace Interpreters
	} // namespace Binary
} // namespace Karkinolution
