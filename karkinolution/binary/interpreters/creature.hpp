#pragma once

#include "karkinolution/binary/interpreters/models/creature.hpp"
#include "karkinolution/organism/entities/creature/constructor.hpp"
#include "karkinolution/organism/entities/genetics/genetic.hpp"
#include "shared/binary/fields.hpp"
#include "shared/core/id.hpp"

#include <array>
#include <cstdlib>
#include <karkinolution/organism/entities/creature/creature.hpp>
#include <shared/binary/interpreters/vec.hpp>
#include <vector>

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
					 * # end position
					 * [26 - name size]
					 * [27 -> 90 - name]
					 * [91 -> 99 - id]
					 *
					 */

					namespace Layout {
						inline std::size_t constexpr MAX_NAME_SIZE = 64;

						using Gender    = KShared::Binary::Field<0, 1>;
						using Specie    = KShared::Binary::Next<Gender, 1>;
						using PositionX = KShared::Binary::Next<Specie, VecSRI::Layout::AXIS_SIZE>;
						using PositionY = KShared::Binary::Next<PositionX, VecSRI::Layout::AXIS_SIZE>;
						using NameSize  = KShared::Binary::Next<PositionY, 1>;
						using Name      = KShared::Binary::Next<NameSize, MAX_NAME_SIZE>;
						using Id        = KShared::Binary::Next<Name, 8>;
						inline constexpr std::size_t TOTAL_SIZE = Id::END;
					} // namespace Layout

					namespace BytesArrays {
						using Creature = std::array<std::byte, Layout::TOTAL_SIZE>;
					} // namespace BytesArrays

					std::byte serialize_gender(const Karkinolution::Organisms::Entities::Creature::Gender &gender);
					std::byte
					serialize_specie(const Karkinolution::Organisms::Entities::Creature::CreatureSpecies &specie);
					std::vector<std::byte> serialize_name(const std::string &name);

					BytesArrays::Creature
					serialize_creature(const Karkinolution::Organisms::Entities::Creature::Creature &creature);

					namespace Request {
						namespace Layout {
							namespace CreateCreature {
								using HasGender   = KShared::Binary::Field<0, 1>;
								using Gender      = KShared::Binary::Next<HasGender, 1>;
								using HasSpecies  = KShared::Binary::Next<Gender, 1>;
								using Species     = KShared::Binary::Next<HasSpecies, 1>;
								using HasPosition = KShared::Binary::Next<Species, 1>;
								using Position    = KShared::Binary::Next<HasPosition, VecSRI::Layout::TOTAL_SIZE>;
								using HasName     = KShared::Binary::Next<Position, 1>;
								using NameSize    = KShared::Binary::Next<HasName, 1>;
								using Name        = KShared::Binary::Next<HasName, Creature::Layout::MAX_NAME_SIZE>;

								inline constexpr std::size_t TOTAL_SIZE = Name::SIZE;
							} // namespace CreateCreature
						} // namespace Layout

						std::array<std::byte, 8> make_see_creature(KShared::Core::Id::BaseIdType id);
						std::array<std::byte, Layout::CreateCreature::TOTAL_SIZE> make_create_creature(
							std::optional<Karkinolution::Organisms::Entities::Creature::Gender>          gender,
							std::optional<Karkinolution::Organisms::Entities::Creature::CreatureSpecies> specie,
							const std::optional<KShared::Math::Vec::Vec2>                               &position,
							const std::optional<std::string>                                            &name);
					} // namespace Request
				} // namespace Creature
			} // namespace SRI

			namespace DSI {
				namespace Creature {
					Karkinolution::Organisms::Entities::Creature::Gender
					get_gender(const std::vector<std::byte> &payload, std::size_t offset);
					Karkinolution::Organisms::Entities::Creature::CreatureSpecies
					get_specie(const std::vector<std::byte> &payload, std::size_t offset);


					uint8_t get_name_size(const std::vector<std::byte> &payload, std::size_t offset);

					std::string                   get_name(const std::vector<std::byte> &payload, std::size_t offset);
					KShared::Core::Id::BaseIdType get_id(const std::vector<std::byte> &payload, std::size_t offset);
					Interpreters::Models::Creature::Creature get_creature(const std::vector<std::byte> &payload,
																		  std::size_t                   offset);

					namespace Request {


						KShared::Core::Id::BaseIdType
						interpret_like_get_creature(const std::vector<std::byte> &payload);
						Karkinolution::Organisms::Entities::Creature::Constructor::CreatureBlueprint
						interpret_like_create_creature(const std::vector<std::byte> &payload);

					} // namespace Request
				} // namespace Creature
			} // namespace DSI
		} // namespace Interpreters
	} // namespace Binary
} // namespace Karkinolution

namespace CreatureSRI = Karkinolution::Binary::Interpreters::SRI::Creature;
namespace CreatureDSI = Karkinolution::Binary::Interpreters::DSI::Creature;
