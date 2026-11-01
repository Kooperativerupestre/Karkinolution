#pragma once
#include "karkinolution/organism/values.hpp"

#include <karkinolution/organism/entities/creature/creature.hpp>
#include <karkinolution/organism/foods/foods.hpp>
#include <karkinolution/organism/nature/grass/grass.hpp>

namespace Karkinolution::Organisms::Entities::Creature::Metabolism::Motor {
	namespace MetabolismMotor {
		void eat(Creature &creature, Karkinolution::Organisms::RawMeat &raw_meat);
		void eat(Creature &creature,
				 Karkinolution::Organisms::Nature::Grass::GrassMatter &grass);
	} // namespace MetabolismMotor
} // namespace Karkinolution::Organisms::Entities::Creature::Metabolism::Motor
