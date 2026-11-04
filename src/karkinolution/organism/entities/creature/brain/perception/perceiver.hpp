#pragma once

#include <karkinolution/organism/entities/corpse/corpse.hpp>
#include <karkinolution/organism/entities/creature/brain/perception/perception.hpp>
#include <karkinolution/organism/entities/creature/creature.hpp>
#include <karkinolution/terrain/soil/soil.hpp>
#include <karkinolution/world/world.hpp>

namespace Karkinolution::Organisms::Entities::Creature::Brain::Perception {
	namespace Perceiver {
		PerceivedCreature perceive(const Creature &target, const Creature &perceiver);
		PerceivedCorpse   perceive(const Karkinolution::Organisms::Entities::Corpse::Corpse &target,
								   const Creature &perceiver);
		PerceivedSoil     perceive(const Karkinolution::Terrain::Soil::SoilPiece &soil,
								   const Creature                                &perceiver);
		PerceivedGrass    perceive(const Karkinolution::Organisms::Nature::Grass::Grass &grass,
								   const Creature                                       &creature);

		Perception perceive(const Creature &perceiver, const Karkinolution::World::World &world);
	} // namespace Perceiver
} // namespace Karkinolution::Organisms::Entities::Creature::Brain::Perception
