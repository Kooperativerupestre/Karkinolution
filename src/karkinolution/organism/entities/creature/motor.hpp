#pragma once
#include "karkinolution/organism/entities/creature/actions/presets.hpp"

#include <karkinolution/organism/entities/creature/brain/perception/perception.hpp>
#include <karkinolution/organism/entities/creature/creature.hpp>
#include <karkinolution/organism/entities/entities.hpp>
#include <karkinolution/world/world.hpp>

namespace Karkinolution::Organisms::Entities::Creature::Brain::Motor {
	namespace BrainMotor {
		void run(Karkinolution::Organisms::Entities::Creature::Brain::Brain &brain,
				 const Creature                                             &creature);
	} // namespace BrainMotor
} // namespace Karkinolution::Organisms::Entities::Creature::Brain::Motor

namespace Karkinolution::Organisms::Entities::Creature::Motor {
	namespace CreatureMotor {
		void run(Creature &creature, Karkinolution::World::World &);
		void update_map(const Creature &creature, Karkinolution::World::World &world);
		void grow(Creature &creature, const Karkinolution::Organisms::Entities::EntitiesRegistry &);
		std::optional<Actions::AllPresets> resolve_presets(
			Creature &creature,
			const Karkinolution::Organisms::Entities::Creature::Brain::Perception::Perception
										&perception,
			Karkinolution::World::World &world);
	} // namespace CreatureMotor
} // namespace Karkinolution::Organisms::Entities::Creature::Motor
