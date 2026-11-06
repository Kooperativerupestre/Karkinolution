#include "karkinolution/core/id.hpp"
#include "karkinolution/math/stats/compile_values.hpp"
#include "karkinolution/organism/entities/creature/creature.hpp"
#include "karkinolution/organism/entities/embryo/embryo.hpp"
#include "karkinolution/organism/entities/genetics/motor.hpp"
#include "karkinolution/organism/entities/identity.hpp"
#include "karkinolution/organism/reproduction/state/state.hpp"
#include "karkinolution/organism/reproduction/state/validator.hpp"
#include "karkinolution/organism/values.hpp"

#include <karkinolution/functional/k_random.hpp>
#include <karkinolution/organism/entities/creature/validator.hpp>
#include <karkinolution/organism/reproduction/state/physiology.hpp>
#include <random>

namespace Karkinolution::Organisms::Entities::Reproduction::State {
	int Physiology::get_children_count(Creature::Creature &creature) {
		std::poisson_distribution<int> distribution(
			creature.genome.creature_genome.reproductive.average_children_count);

		return distribution(KRandom::gen);
		// Poisson distribution is used here because repeated samples converge to the specified
		// mean.
	}

	float Physiology::needed_energy_of_all_embryos(const ReproductiveState                          &organ,
												   const Karkinolution::Organisms::OrganismRegistry &organisms) {
		Validator::is_pregnant(organ);

		float       total_energy = 0.0f;
		const auto &uterus       = std::get<Uterus::Uterus>(organ.state);

		for (auto id : uterus.get_pregnant_uterus().embryos) {
			const auto &embryo =
				organisms.entities_registry.at_embryo(Karkinolution::Organisms::Entities::create_embryo_id(id));
			total_energy += (embryo.energy.max() - embryo.energy.value());
		}
		return total_energy;
	}

	Entities::Embryo::Embryo Physiology::generate_embryo(const Entities::Creature::Creature &female,
														 const Entities::Creature::Creature &male) {
		Karkinolution::Organisms::Entities::Creature::Validator::is_same_specie(female, male);

		Entities::Genetic::Genome new_genome = Entities::Genetic::Motor::crossover(female.genome, male.genome);

		auto average_max_energy = new_genome.calculate_embryo_average_max_energy();
		auto average_area       = new_genome.calculate_embryo_average_area();
		auto average_max_life   = new_genome.calculate_embryo_average_life();

		auto init_average_max_energy = average_max_energy / 7.0f;
		Area init_average_area       = average_area.value / 5.0f;
		auto init_average_max_life   = average_max_life / 5.0f;

		Energy energy(init_average_max_energy / 2.0f, init_average_max_energy);
		Life   life(init_average_max_life / 2.0f, init_average_max_life);

		return Embryo::Embryo{
			.id     = Core::Id::gen_id(),
			.energy = energy,
			.life   = life,
			.health = new_genome.embryo_genome.body.average_health.value(),
			.area   = Area{init_average_area},
			.genome = std::move(new_genome),
			.age    = 0,
		};
	}
} // namespace Karkinolution::Organisms::Entities::Reproduction::State
