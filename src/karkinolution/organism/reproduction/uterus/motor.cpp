#include "karkinolution/math/stats/compile_values.hpp"
#include "karkinolution/organism/reproduction/state/state.hpp"

#include <karkinolution/organism/entities/creature/creature.hpp>
#include <karkinolution/organism/entities/embryo/embryo.hpp>
#include <karkinolution/organism/reproduction/state/validator.hpp>
#include <karkinolution/organism/reproduction/uterus/motor.hpp>
#include <karkinolution/organism/reproduction/uterus/physiology.hpp>
#include <karkinolution/organism/reproduction/uterus/uterus.hpp>

void UterusMotor::transfer_energy_to_uterus(Creature &creature) {
	transfer_energy_to_uterus(creature, NormalizedValue<float>(0.2f), NormalizedValue<float>(0.5f));
}

void UterusMotor::transfer_energy_to_uterus(Creature &creature, float energy, float reserved) {
	ReproductionValidator::has_uterus(creature.reproduction);

	auto &uterus = std::get<Uterus>(creature.reproduction.state);

	const float uterus_remaining = uterus.energy.max() - uterus.energy.value();

	const float real_energy_needed = std::min(uterus_remaining, energy + reserved);

	const float energy_used = std::min(energy, real_energy_needed);

	const float reserved_used = real_energy_needed - energy_used;
	uterus.energy += real_energy_needed;
	creature.body.metabolism.energy -= energy_used;
	creature.body.metabolism.reserved -= reserved;
}

void UterusMotor::transfer_energy_to_uterus(Creature              &creature,
											NormalizedValue<float> energy_ratio,
											NormalizedValue<float> reserved_ratio) {
	transfer_energy_to_uterus(creature,
							  energy_ratio.value() * creature.body.metabolism.energy.value(),
							  reserved_ratio.value() * creature.body.metabolism.reserved.value());
}
