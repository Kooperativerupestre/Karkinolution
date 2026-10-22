#pragma once
#include "karkinolution/core/id.hpp"
#include "karkinolution/organism/entities/creature/constructor.hpp"
#include "karkinolution/organism/entities/genetics/genetic.hpp"

#include <karkinolution/organism/entities/creature/creature.hpp>
#include <karkinolution/world/world.hpp>

namespace CreatureAPI {
	const Creature* see_creature(const World &world, BaseIdType id);
	const Creature* make_creature(World                            &world,
								  std::optional<Gender>             gender,
								  std::optional<CreatureSpecies>    specie,
								  const std::optional<Vec3>        &position,
								  const std::optional<std::string> &name);
} // namespace CreatureAPI
