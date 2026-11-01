#pragma once
#include "karkinolution/organism/values.hpp"

#include <karkinolution/organism/entities/creature/creature.hpp>
#include <karkinolution/organism/values.hpp>

namespace Karkinolution::Organisms::Entities::Creature::Attack::Physiology {
	namespace AttackPhysiology {
		Karkinolution::Organisms::Damage calculate_damage(const Creature &creature);
	} // namespace AttackPhysiology
} // namespace Karkinolution::Organisms::Entities::Creature::Attack::Physiology
