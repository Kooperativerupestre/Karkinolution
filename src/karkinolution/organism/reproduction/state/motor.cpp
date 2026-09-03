#include "karkinolution/math/stats/compile_values.hpp"
#include "karkinolution/organism/entities/embryo/embryo.hpp"
#include "karkinolution/organism/reproduction/gestation.hpp"
#include "karkinolution/organism/reproduction/state/physiology.hpp"
#include "karkinolution/organism/reproduction/state/state.hpp"

#include <karkinolution/organism/entities/creature/creature.hpp>
#include <karkinolution/organism/reproduction/state/motor.hpp>
#include <karkinolution/organism/reproduction/state/validator.hpp>
#include <karkinolution/organism/reproduction/uterus/motor.hpp>
#include <karkinolution/organism/reproduction/uterus/uterus.hpp>

void ReproductionStateMotor::run(Creature &creature) {
	ReproductionValidator::has_uterus(creature.reproduction);

	auto &uterus = std::get<Uterus>(creature.reproduction.state);

	if (uterus.is_pregnant()) {
		UterusMotor::transfer_energy_to_uterus(creature);
		uterus.get_pregnant_uterus().gestation.pass();
	}
}

void ReproductionStateMotor::prepair_to_conceive(Creature &creature) {
	ReproductionValidator::has_uterus(creature.reproduction);
	ReproductionValidator::is_not_pregnant(creature.reproduction);

	auto &uterus = std::get<Uterus>(creature.reproduction.state);

	UterusMotor::transfer_energy_to_uterus(creature,
										   NormalizedValue<float>(0.3f),
										   NormalizedValue<float>(0.5f));
	creature.body.reproductive.fertility.zero();
	uterus.state = PregnantUterus{
		.embryos = {},
		.gestation =
			Gestation{0,
					  static_cast<uint16_t>(
						  creature.genome.creature_genome.reproductive.average_gestation_limit)},
		.born_count = 0,
	};
}

std::variant<Embryo, ConceiveOutput> ReproductionStateMotor::conceive(Creature &female,
																	  Creature &male) {
	ReproductionValidator::has_uterus(female.reproduction);
	auto &uterus = std::get<Uterus>(female.reproduction.state);

	if (uterus.is_pregnant()) {
		return ConceiveOutput::ALREADY_PREGNANT;
	}

	ReproductionStateMotor::prepair_to_conceive(female);

	auto &pregnant_uterus = uterus.get_pregnant_uterus();

	int children_count = ReproductionStatePhysiology::get_children_count(female);

	for (int i = 0; i < children_count; i++) {
		Embryo embryo = ReproductionStatePhysiology::generate_embryo(female, male);
		pregnant_uterus.embryos.push_back(embryo.id);
		return embryo;
	}
	return ConceiveOutput::OK;
	// add: genome -> embryo
}