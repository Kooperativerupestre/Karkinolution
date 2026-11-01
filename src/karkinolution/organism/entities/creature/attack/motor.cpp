
#include <karkinolution/organism/entities/creature/attack/motor.hpp>
#include <karkinolution/organism/entities/creature/attack/physiology.hpp>
#include <karkinolution/organism/entities/creature/creature.hpp>
#include <karkinolution/organism/values.hpp>

namespace Karkinolution::Organisms::Entities::Creature::Attack::Motor {
	Karkinolution::Organisms::Damage AttackMotor::attack(const Creature &attacker,
													 Creature &target) {
		const Damage damage =
			Karkinolution::Organisms::Entities::Creature::Attack::Physiology::AttackPhysiology::
				calculate_damage(attacker);

		target.body.vital.life -= damage.value;

		return damage;
	}
} // namespace Karkinolution::Organisms::Entities::Creature::Attack::Motor
