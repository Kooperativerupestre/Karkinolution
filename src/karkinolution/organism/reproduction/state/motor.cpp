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

namespace Karkinolution::Organisms::Entities::Reproduction::State::Motor {

	void run(Karkinolution::Organisms::Entities::Creature::Creature &creature) {
		Validator::has_uterus(creature.body.reproduction.state);

		auto &uterus = std::get<Karkinolution::Organisms::Entities::Reproduction::Uterus::Uterus>(
			creature.body.reproduction.state.state);
		if (uterus.is_pregnant()) {
			Karkinolution::Organisms::Entities::Reproduction::Uterus::Motor::transfer_energy_to_uterus(creature);
			uterus.get_pregnant_uterus().gestation.pass();
		}
	}

	void prepair_to_conceive(Karkinolution::Organisms::Entities::Creature::Creature &creature) {
		Validator::has_uterus(creature.body.reproduction.state);
		Validator::is_not_pregnant(creature.body.reproduction.state);

		auto &uterus = std::get<Karkinolution::Organisms::Entities::Reproduction::Uterus::Uterus>(
			creature.body.reproduction.state.state);

		auto number_of_children = State::Physiology::get_children_count(creature);
		Karkinolution::Organisms::Entities::Reproduction::Uterus::Motor::transfer_energy_to_uterus(
			creature,
			Karkinolution::Math::LV::CT::NormalizedValue<float>(0.3f),
			Karkinolution::Math::LV::CT::NormalizedValue<float>(0.5f));
		creature.body.reproduction.fertility.zero();
		uterus.state = Karkinolution::Organisms::Entities::Reproduction::Uterus::PregnantUterus{
			.embryos = {},
			.gestation =
				Karkinolution::Organisms::Entities::Reproduction::Uterus::Gestation{
					0,
					static_cast<uint16_t>(creature.genome.creature_genome.reproductive.average_gestation_limit)},
			.born_count = Karkinolution::Organisms::Entities::Reproduction::Uterus::BornCount{number_of_children},
		};
	}

	std::variant<Karkinolution::Organisms::Entities::Embryo::Embryo, ConceiveOutput>
	conceive(Karkinolution::Organisms::Entities::Creature::Creature &female,
			 Karkinolution::Organisms::Entities::Creature::Creature &male) {
		Validator::has_uterus(female.body.reproduction.state);
		auto &uterus = std::get<Karkinolution::Organisms::Entities::Reproduction::Uterus::Uterus>(
			female.body.reproduction.state.state);

		if (uterus.is_pregnant()) {
			return ConceiveOutput::ALREADY_PREGNANT;
		}

		prepair_to_conceive(female);

		auto &pregnant_uterus = uterus.get_pregnant_uterus();

		int children_count = State::Physiology::get_children_count(female);

		for (int i = 0; i < children_count; i++) {
			Karkinolution::Organisms::Entities::Embryo::Embryo embryo =
				State::Physiology::generate_embryo(female, male);
			pregnant_uterus.embryos.push_back(embryo.id);
			return embryo;
		}
		return ConceiveOutput::OK;
		// add: genome -> embryo
	}

} // namespace Karkinolution::Organisms::Entities::Reproduction::State::Motor
