#pragma once
#include "karkinolution/math/stats/compile_values.hpp"
#include "karkinolution/organism/reproduction/state/state.hpp"

#include <karkinolution/organism/entities/creature/creature.hpp>
#include <karkinolution/organism/entities/embryo/embryo.hpp>
#include <karkinolution/organism/reproduction/uterus/physiology.hpp>
#include <karkinolution/organism/reproduction/uterus/uterus.hpp>

namespace UterusMotor {
void transfer_energy_to_uterus(Creature &creature);
void transfer_energy_to_uterus(Creature &creature, float energy, float reserved);
void transfer_energy_to_uterus(Creature              &creature,
							   NormalizedValue<float> energy_ratio,
							   NormalizedValue<float> reserved_ratio);
} // namespace UterusMotor