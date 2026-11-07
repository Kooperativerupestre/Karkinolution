#pragma once
#include <karkinolution/organism/entities/creature/attack/physiology.hpp>
#include <karkinolution/organism/entities/creature/creature.hpp>
#include <karkinolution/organism/values.hpp>

namespace Karkinolution::Organisms::Entities::Creature::Attack::Motor {
	namespace AttackMotor {
		Karkinolution::Organisms::Damage attack(const Creature &attacker, Creature &target);
	} // namespace AttackMotor
} // namespace Karkinolution::Organisms::Entities::Creature::Attack::Motor
