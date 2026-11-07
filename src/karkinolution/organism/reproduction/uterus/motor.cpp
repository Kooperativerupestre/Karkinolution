#include "shared/math/stats/compile_values.hpp"
#include "karkinolution/organism/reproduction/state/state.hpp"

#include <karkinolution/organism/entities/creature/creature.hpp>
#include <karkinolution/organism/entities/embryo/embryo.hpp>
#include <karkinolution/organism/reproduction/state/validator.hpp>
#include <karkinolution/organism/reproduction/uterus/motor.hpp>
#include <karkinolution/organism/reproduction/uterus/physiology.hpp>
#include <karkinolution/organism/reproduction/uterus/uterus.hpp>

namespace Karkinolution::Organisms::Entities::Reproduction::Uterus::Motor {

	void transfer_energy_to_uterus(Creature::Creature &creature) {
		transfer_energy_to_uterus(creature, NormalizedValue<float>(0.2f), NormalizedValue<float>(0.5f));
	}

	void transfer_energy_to_uterus(Creature::Creature &creature, float energy, float reserved) {
		State::Validator::has_uterus(creature.body.reproduction.state);

		auto &uterus = std::get<Uterus>(creature.body.reproduction.state.state);

		const float uterus_remaining = uterus.energy.max() - uterus.energy.value();

		const float real_energy_needed = std::min(uterus_remaining, energy + reserved);

		const float energy_used = std::min(energy, real_energy_needed);

		const float reserved_used = real_energy_needed - energy_used;
		uterus.energy += real_energy_needed;
		creature.body.metabolism.energy -= energy_used;
		creature.body.metabolism.reserved -= reserved;
	}

	void transfer_energy_to_uterus(Creature::Creature    &creature,
								   NormalizedValue<float> energy_ratio,
								   NormalizedValue<float> reserved_ratio) {
		transfer_energy_to_uterus(creature,
								  energy_ratio.value() * creature.body.metabolism.energy.value(),
								  reserved_ratio.value() * creature.body.metabolism.reserved.value());
	}

} // namespace Karkinolution::Organisms::Entities::Reproduction::Uterus::Motor
