#pragma once
#include "karkinolution/organism/stats.hpp"

#include <karkinolution/organism/entities/creature/creature.hpp>
#include <karkinolution/organism/stats.hpp>

using OrganismStats::Body::Damage;

namespace AttackPhysiology {
	Damage calculate_damage(const Creature &creature);
} // namespace AttackPhysiology