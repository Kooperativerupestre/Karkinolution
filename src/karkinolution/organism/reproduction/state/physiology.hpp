#pragma once
#include "karkinolution/organism/entities/embryo/embryo.hpp"

#include <karkinolution/organism/entities/creature/creature.hpp>
#include <karkinolution/organism/registry.hpp>
#include <karkinolution/organism/reproduction/state/state.hpp>
#include <karkinolution/organism/reproduction/uterus/uterus.hpp>

namespace Karkinolution::Organisms::Entities::Reproduction::State::Physiology {

	int get_children_count(Entities::Creature::Creature &creature);
	// not deterministic
	float needed_energy_of_all_embryos(const ReproductiveState                          &organ,
									   const Karkinolution::Organisms::OrganismRegistry &organisms);

	Embryo::Embryo generate_embryo(const Creature::Creature &female,
								   const Creature::Creature &male);
} // namespace Karkinolution::Organisms::Entities::Reproduction::State::Physiology
