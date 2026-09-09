#pragma once
#include <karkinolution/organism/entities/creature/creature.hpp>
#include <karkinolution/organism/foods/foods.hpp>

namespace MetabolismMotor {
	void eat(Creature &creature, RawMeat &raw_meat);
	void eat(Creature &creature, Grass &grass);
} // namespace MetabolismMotor