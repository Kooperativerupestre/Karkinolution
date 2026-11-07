#pragma once
#include "shared/math/stats/compile_values.hpp"
#include "karkinolution/organism/reproduction/state/state.hpp"

#include <karkinolution/organism/entities/creature/creature.hpp>
#include <karkinolution/organism/entities/embryo/embryo.hpp>
#include <karkinolution/organism/reproduction/uterus/physiology.hpp>
#include <karkinolution/organism/reproduction/uterus/uterus.hpp>

namespace Karkinolution {
	namespace Organisms {
		namespace Entities {
			namespace Reproduction {
				namespace Uterus {
					namespace Motor {
						void transfer_energy_to_uterus(Creature::Creature &creature);
						void transfer_energy_to_uterus(Creature::Creature &creature, float energy, float reserved);
						void transfer_energy_to_uterus(Creature::Creature    &creature,
													   NormalizedValue<float> energy_ratio,
													   NormalizedValue<float> reserved_ratio);
					} // namespace Motor
				} // namespace Uterus
			} // namespace Reproduction
		} // namespace Entities
	} // namespace Organisms
} // namespace Karkinolution
