#pragma once


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
	 */
	inline constexpr size_t CREATURE_BYTES = 26;
	inline constexpr size_t GENDER_BYTES   = 1;

	inline constexpr size_t SPECIE_BYTES = 1;


	inline constexpr size_t POSITION_AXIS_BYTES = 8;
	inline constexpr size_t POSITION_BYTES      = POSITION_AXIS_BYTES * 3;


	inline constexpr size_t TO_GET_GENDER_OFFSET     = 0;
	inline constexpr size_t TO_GET_SPECIE_OFFSET     = GENDER_BYTES;
	inline constexpr size_t TO_GET_POSITION_X_OFFSET = GENDER_BYTES + SPECIE_BYTES;
	inline constexpr size_t TO_GET_POSITION_Y_OFFSET =
		GENDER_BYTES + SPECIE_BYTES + POSITION_AXIS_BYTES;
	inline constexpr size_t TO_GET_POSITION_Z_OFFSET =
		GENDER_BYTES + SPECIE_BYTES + POSITION_AXIS_BYTES * 2;

	static_assert(CREATURE_BYTES == TO_GET_POSITION_Z_OFFSET + POSITION_AXIS_BYTES);

	using CreatureBytes = std::array<std::byte, CREATURE_BYTES>;
	CreatureBytes serialize_creature(const Creature &creature);
} // namespace CreatureSRI
