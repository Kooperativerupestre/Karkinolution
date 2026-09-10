#pragma once
#include "karkinolution/organism/stats.hpp"

#include <karkinolution/organism/entities/creature/creature.hpp>
#include <karkinolution/organism/foods/foods.hpp>
#include <karkinolution/organism/nature/grass/grass.hpp>

using OrganismStats::Body::RawMeat;

namespace MetabolismMotor {
	void eat(Creature &creature, RawMeat &raw_meat);
	void eat(Creature &creature, GrassMatter &grass);
} // namespace MetabolismMotor