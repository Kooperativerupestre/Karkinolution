#pragma once
#include <karkinolution/organism/entities/creature/attack/physiology.hpp>
#include <karkinolution/organism/entities/creature/creature.hpp>
#include <karkinolution/organism/stats.hpp>

namespace AttackMotor {
OrganismStats::Body::Damage attack(const Creature &attacker, Creature &target);
} // namespace AttackMotor