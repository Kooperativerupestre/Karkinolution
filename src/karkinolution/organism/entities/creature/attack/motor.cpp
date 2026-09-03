
#include <karkinolution/organism/entities/creature/attack/motor.hpp>
#include <karkinolution/organism/entities/creature/attack/physiology.hpp>
#include <karkinolution/organism/entities/creature/creature.hpp>
#include <karkinolution/organism/stats.hpp>

OrganismStats::Body::Damage AttackMotor::attack(const Creature &attacker, Creature &target) {
	const Damage damage = AttackPhysiology::calculate_damage(attacker);

	target.body.vital.life -= damage.value;

	target.body.skeleton.quality -= (target.body.vital.life.max() / damage.value) * 0.75f;
	return damage;
}