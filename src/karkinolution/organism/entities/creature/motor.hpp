#pragma once
#include <karkinolution/organism/entities/creature/creature.hpp>
#include <karkinolution/organism/entities/entities.hpp>
#include <karkinolution/organism/registry.hpp>

namespace CreatureMotor {
    void run(Creature&creature, const OrganismRegistry&);
    void grow(Creature&creature, const OrganismRegistry&);
}
