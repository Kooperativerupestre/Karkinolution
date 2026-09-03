#include "karkinolution/core/id_generator.hpp"
#include "karkinolution/math/stats/compile_values.hpp"
#include "karkinolution/organism/entities/genetics/motor.hpp"
#include "karkinolution/organism/entities/identity.hpp"
#include "karkinolution/organism/reproduction/state/state.hpp"
#include "karkinolution/organism/reproduction/state/validator.hpp"
#include "karkinolution/organism/stats.hpp"

#include <karkinolution/organism/entities/creature/validator.hpp>
#include <karkinolution/organism/reproduction/state/physiology.hpp>
#include <karkinolution/utils/k_random.hpp>
#include <random>

int ReproductionStatePhysiology::get_children_count(Creature &creature) {
	std::poisson_distribution<int> distribution(
		creature.genome.creature_genome.reproductive.average_children_count);

	return distribution(gen);
	// Poisson distribution is used here because repeated samples converge to the specified mean.
}

float ReproductionStatePhysiology::needed_energy_of_all_embryos(const ReproductiveState &organ,
																const OrganismRegistry &organisms) {
	ReproductionValidator::is_pregnant(organ);

	float       total_energy = 0.0f;
	const auto &uterus       = std::get<Uterus>(organ.state);

	for (auto id : uterus.get_pregnant_uterus().embryos) {
		const auto &embryo = organisms.entities.at_embryo(IDF::create_embryo_id(id));
		total_energy += (embryo.energy.max() - embryo.energy.value());
	}
	return total_energy;
}

Embryo ReproductionStatePhysiology::generate_embryo(const Creature &female, const Creature &male) {
	CreatureValidator::is_same_specie(female, male);

	Genome new_genome = GenomeMotor::crossover(female.genome, male.genome);

	auto average_max_energy = new_genome.calculate_embryo_average_max_energy();
	auto average_volume     = new_genome.calculate_embryo_average_volume();
	auto average_max_life   = new_genome.calculate_embryo_average_life();

	auto   init_average_max_energy = average_max_energy / 7.0f;
	Volume init_average_volume     = average_volume.value / 5.0f;
	auto   init_average_max_life   = average_max_life / 5.0f;

	Energy energy(init_average_max_energy / 2.0f, init_average_max_energy);
	Life   life(init_average_max_life / 2.0f, init_average_max_life);

	return Embryo{
		.id     = gen_id(),
		.energy = std::move(energy),
		.life   = std::move(life),
		.health = new_genome.embryo_genome.body.average_health.value(),
		.volume = Volume{init_average_volume},
		.genome = std::move(new_genome),
		.age    = 0,
	};
}