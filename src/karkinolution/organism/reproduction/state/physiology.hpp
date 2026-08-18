#pragma once
#include <karkinolution/organism/entities/creature/creature.hpp>
#include <karkinolution/organism/reproduction/uterus/uterus.hpp>
#include <karkinolution/organism/reproduction/state/state.hpp>

namespace ReproductionOrganPhysiology {
    int get_children_count(Creature&creature);
    // not deterministic
}