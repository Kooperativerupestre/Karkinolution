#pragma once
#include <karkinolution/organism/entities/creature/creature.hpp>
#include <karkinolution/organism/entities/entities.hpp>
#include <karkinolution/organism/registry.hpp>
#include <karkinolution/terrain/world.hpp>

namespace CreatureMotor {
void run(Creature& creature, World&);
void grow(Creature& creature, const OrganismRegistry&);
} // namespace CreatureMotor
