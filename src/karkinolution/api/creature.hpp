#pragma once
#include "karkinolution/core/id.hpp"

#include <karkinolution/organism/entities/creature/creature.hpp>
#include <karkinolution/world/world.hpp>

namespace CreatureAPI {
	const Creature* get_creature(const World &world, BaseIdType id);
} // namespace CreatureAPI