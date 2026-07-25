#pragma once
#include <karkinolution/organism/ontology.hpp>
#include <karkinolution/core/stats.hpp>
#include <karkinolution/organism/creatures.hpp>

namespace DeathSystem {
    Corpse generate_corpse(const Creature&creature);
    bool is_dead(const Creature&creature);
};