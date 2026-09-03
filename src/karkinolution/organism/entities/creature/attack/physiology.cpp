#include <karkinolution/organism/entities/creature/attack/physiology.hpp>
#include <karkinolution/organism/entities/creature/creature.hpp>
#include <karkinolution/organism/stats.hpp>

using OrganismStats::Body::Damage;

Damage AttackPhysiology::calculate_damage(const Creature &creature) {
	return creature.body.strength().value;
}