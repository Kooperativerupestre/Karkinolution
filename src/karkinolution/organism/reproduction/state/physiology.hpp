#pragma once
#include <karkinolution/organism/entities/creature/creature.hpp>
#include <karkinolution/organism/reproduction/uterus/uterus.hpp>
#include <karkinolution/organism/reproduction/state/state.hpp>
#include <karkinolution/organism/registry.hpp>

namespace ReproductionOrganPhysiology {
    int get_children_count(Creature&creature);
        // not deterministic
    float needed_energy_of_all_embryos(const ReproductionOrgan&organ, const OrganismRegistry&organisms);

}