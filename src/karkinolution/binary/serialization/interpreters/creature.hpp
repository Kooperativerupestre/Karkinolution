#pragma once


#include "math/ vec.hpp"

#include <array>
#include <karkinolution/organism/entities/creature/creature.hpp>

namespace CreatureSRI {


	/* CREATURE PROTOCOL
	 * [0 - Gender]
	 * [1 - Specie]
	 *
	 * # begin position
	 * [2 -> 9 - x]
	 * [10 -> 17 - y]
	 * [18 -> 25 - z]
	 * # end position
	 *
	 * [26 -> 70 - name]
	 *
	 */
	inline constexpr size_t CREATURE_BYTES = 70;
	inline constexpr size_t GENDER_BYTES   = 1;

	inline constexpr size_t SPECIE_BYTES = 1;


	inline constexpr size_t POSITION_AXIS_BYTES = VecSRI::AXIS_BYTES;
	inline constexpr size_t POSITION_BYTES      = POSITION_AXIS_BYTES * 3;
	inline constexpr size_t NAME_BYTES          = 44;

	inline constexpr size_t TO_GET_GENDER_OFFSET     = 0;
	inline constexpr size_t TO_GET_SPECIE_OFFSET     = GENDER_BYTES;
	inline constexpr size_t TO_GET_POSITION_X_OFFSET = GENDER_BYTES + SPECIE_BYTES;
	inline constexpr size_t TO_GET_POSITION_Y_OFFSET =
		GENDER_BYTES + SPECIE_BYTES + POSITION_AXIS_BYTES;
	inline constexpr size_t TO_GET_POSITION_Z_OFFSET =
		GENDER_BYTES + SPECIE_BYTES + POSITION_AXIS_BYTES * 2;

	inline constexpr size_t TO_GET_POSITION_OFFSET = GENDER_BYTES + SPECIE_BYTES;
	inline constexpr size_t TO_GET_NAME_OFFSET =
		GENDER_BYTES + SPECIE_BYTES + POSITION_AXIS_BYTES * 3;

	static_assert(CREATURE_BYTES == TO_GET_NAME_OFFSET + NAME_BYTES);

	using NameBytes     = std::array<std::byte, NAME_BYTES>;
	using CreatureBytes = std::array<std::byte, CREATURE_BYTES>;

	std::byte serialize_gender(const Gender &gender);
	std::byte serialize_specie(const CreatureSpecies &specie);
	NameBytes serialize_name(const std::string &name);

	CreatureBytes serialize_creature(const Creature &creature);
} // namespace CreatureSRI
