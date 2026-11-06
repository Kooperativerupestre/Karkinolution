#pragma once
#include "karkinolution/core/id.hpp"
#include "karkinolution/organism/entities/creature/constructor.hpp"
#include "karkinolution/organism/entities/genetics/genetic.hpp"

#include <karkinolution/organism/entities/creature/creature.hpp>
#include <karkinolution/world/world.hpp>

namespace Karkinolution::API::Creature {
	const Karkinolution::Organisms::Entities::Creature::Creature*
	see_creature(const Karkinolution::World::World &world, Karkinolution::Core::Id::BaseIdType id);
	const Karkinolution::Organisms::Entities::Creature::Creature* make_creature(
		Karkinolution::World::World                                                 &world,
		std::optional<Karkinolution::Organisms::Entities::Creature::Gender>          gender,
		std::optional<Karkinolution::Organisms::Entities::Creature::CreatureSpecies> specie,
		const std::optional<Karkinolution::Math::Vec::Vec2>                         &position,
		const std::optional<std::string>                                            &name);
} // namespace Karkinolution::API::Creature
