#pragma once
#include <karkinolution/organism/entities/creature/creature.hpp>
#include <karkinolution/organism/registry.hpp>
#include <karkinolution/organism/reproduction/state/state.hpp>
#include <karkinolution/organism/reproduction/uterus/uterus.hpp>

namespace ReproductionStatePhysiology {
int get_children_count(Creature& creature);
// not deterministic
float needed_energy_of_all_embryos(const ReproductiveState& organ,
                                   const OrganismRegistry& organisms);

Embryo generate_embryo(const Creature& female, const Creature& male);
} // namespace ReproductionStatePhysiology