#pragma once

#include "karkinolution/binary/fields.hpp"
#include "karkinolution/core/id.hpp"
#include "karkinolution/organism/entities/creature/constructor.hpp"
#include "karkinolution/organism/entities/genetics/genetic.hpp"

#include <array>
#include <cstdlib>
#include <karkinolution/binary/interpreters/vec.hpp>
#include <karkinolution/organism/entities/creature/creature.hpp>
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
						namespace Layout {
							namespace CreateCreature {
								using HasGender   = Field<0, 1>;
								using Gender      = Next<HasGender, 1>;
								using HasSpecies  = Next<Gender, 1>;
								using Species     = Next<HasSpecies, 1>;
								using HasPosition = Next<Species, 1>;
								using Position    = Next<HasPosition, 24>;
								using HasName     = Next<Position, 1>;
								using NameSize    = Next<HasName, 1>;
								using Name        = Next<HasName, Creature::Layout::MAX_NAME_SIZE>;

								inline constexpr std::size_t TOTAL_SIZE = Name::SIZE;
							} // namespace CreateCreature
						} // namespace Layout

						std::array<std::byte, 8> make_see_creature(BaseIdType id);
						std::array<std::byte, Layout::CreateCreature::TOTAL_SIZE>
						make_create_creature(std::optional<Gender>             gender,
											 std::optional<CreatureSpecies>    specie,
											 const std::optional<Vec3>        &position,
											 const std::optional<std::string> &name);
					} // namespace Request
				} // namespace Creature
			} // namespace SRI

			namespace DSI {
				namespace Creature {
					namespace Models {
						struct Creature {
								Gender          gender;
								CreatureSpecies specie;
								Vec3            position;
								std::string     name;
								BaseIdType      id;
						};

					} // namespace Models

					Gender get_gender(const std::vector<std::byte> &payload, std::size_t offset);
					CreatureSpecies get_specie(const std::vector<std::byte> &payload,
											   std::size_t                   offset);


					uint8_t get_name_size(const std::vector<std::byte> &payload,
										  std::size_t                   offset);

					std::string get_name(const std::vector<std::byte> &payload, std::size_t offset);
					BaseIdType  get_id(const std::vector<std::byte> &payload, std::size_t offset);
					Models::Creature get_creature(const std::vector<std::byte> &payload,
												  std::size_t                   offset);

					namespace Request {


						BaseIdType
						interpret_like_get_creature(const std::vector<std::byte> &payload);
						CreatureBlueprint
						interpret_like_create_creature(const std::vector<std::byte> &payload);

					} // namespace Request
				} // namespace Creature
			} // namespace DSI
		} // namespace Interpreters
	} // namespace Binary
} // namespace Karkinolution

namespace CreatureSRI = Karkinolution::Binary::Interpreters::SRI::Creature;
namespace CreatureDSI = Karkinolution::Binary::Interpreters::DSI::Creature;
