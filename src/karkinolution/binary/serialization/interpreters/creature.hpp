#pragma once


#include <array>
#include <karkinolution/organism/entities/creature/creature.hpp>

namespace CreatureSRI {
	inline constexpr size_t CREATURE_SIZE = 2;
	using CreatureBytes                   = std::array<std::byte, CREATURE_SIZE>;
	CreatureBytes serialize_creature(const Creature &creature);
} // namespace CreatureSRI
