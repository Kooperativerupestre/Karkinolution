#include <karkinolution/organism/entities/creature/attack/physiology.hpp>
#include <karkinolution/organism/entities/creature/creature.hpp>
#include <karkinolution/organism/values.hpp>

namespace Karkinolution::Organisms::Entities::Creature::Attack::Physiology {
	Karkinolution::Organisms::Damage
	AttackPhysiology::calculate_damage(const Creature &creature) {
		return creature.strength().value;
	}
} // namespace Karkinolution::Organisms::Entities::Creature::Attack::Physiology
