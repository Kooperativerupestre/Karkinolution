#include "karkinolution/organism/entities/creature/brain/intents.hpp"
#include "karkinolution/organism/registry.hpp"

#include <karkinolution/functional/k_random.hpp>
#include <karkinolution/math/stats/compile_values.hpp>
#include <karkinolution/organism/entities/creature/creature.hpp>
#include <karkinolution/organism/entities/creature/ontology.hpp>
#include <karkinolution/organism/entities/creature/physiology.hpp>
#include <karkinolution/organism/entities/entities.hpp>
#include <karkinolution/organism/entities/genetics/genetic.hpp>
#include <karkinolution/organism/foods/foods.hpp>
#include <karkinolution/organism/reproduction/state/state.hpp>
#include <karkinolution/organism/values.hpp>
#include <karkinolution/utils/namegenerator.hpp>
#include <utility>

namespace Karkinolution::Organisms::Entities::Creature::Physiology {

	// ============================================================================
	// CreatureGrowingPhysiology
	// ============================================================================

	PhysiologyGrowTrade Growing::get_new_max_energy_increment(
		const Creature                                             &creature,
		const Karkinolution::Organisms::Entities::EntitiesRegistry &organism) {
		const auto &metabolism_genome = creature.genome.creature_genome.metabolism;
		const auto &metabolism        = creature.body.metabolism;

		const float max_energy = metabolism_genome.max_energy;

		const NormalizedValue<float> remaining = 1.0f - metabolism.energy.max() / max_energy;

		const GrowthRate growth = metabolism_genome.growth_rates.max_energy;

		const float gain = remaining.value() * max_energy * growth.value;

		const float reserved_energy_cost = gain;

		return PhysiologyGrowTrade{.gain = gain,
								   .cost = GrowCost{.reserved_energy = reserved_energy_cost}};
	}

	PhysiologyGrowTrade Growing::get_new_max_energy_reserved_increment(
		const Creature                                             &creature,
		const Karkinolution::Organisms::Entities::EntitiesRegistry &organisms) {
		const auto &metabolism_genome = creature.genome.creature_genome.metabolism;
		const auto &metabolism        = creature.body.metabolism;

		const float max_reserved_energy = metabolism_genome.max_reserved_energy;

		const NormalizedValue<float> remaining =
			1.0f - metabolism.reserved.max() / max_reserved_energy;

		const GrowthRate growth = metabolism_genome.growth_rates.max_reserved_energy;

		const float gain = remaining.value() * max_reserved_energy * growth.value;

		const float reserved_energy_cost = gain;

		return PhysiologyGrowTrade{.gain = gain,
								   .cost = GrowCost{.reserved_energy = reserved_energy_cost}};
	}

	PhysiologyGrowTrade Growing::get_new_max_life_increment(
		const Creature                                             &creature,
		const Karkinolution::Organisms::Entities::EntitiesRegistry &organisms) {
		const auto &vital_genome = creature.genome.creature_genome.vital;
		const auto &body         = creature.body;

		const float max_life = vital_genome.max_life;

		const NormalizedValue<float> remaining = 1.0f - body.vital.life.max() / max_life;

		const GrowthRate growth = vital_genome.growth_rates.life.value;

		const NormalizedValue<float> nutrition = 0.2f + body.metabolism.reserved.ratio().value();

		const float gain = remaining.value() * max_life * growth.value * nutrition.value();

		const float reserved_energy_cost = gain;

		return PhysiologyGrowTrade{.gain = gain,
								   .cost = GrowCost{.reserved_energy = reserved_energy_cost}};
	}

	float Growing::get_new_lateral_increment(
		const Creature                                             &creature,
		const Karkinolution::Organisms::Entities::EntitiesRegistry &organisms) {
		const auto &vital_genome      = creature.genome.creature_genome.vital;
		const auto &morphology_genome = creature.genome.creature_genome.morphology;
		const auto &body              = creature.body;

		const NormalizedValue<float> remaining =
			1.0f - body.morphology.size.lateral.value / morphology_genome.average_lateral.value;
		const auto muscle_factor = body.vital.life.ratio().value();

		return remaining.value() * muscle_factor;
	}

	float Growing::get_new_depth_increment(
		const Creature                                             &creature,
		const Karkinolution::Organisms::Entities::EntitiesRegistry &organisms) {
		const auto &vital_genome      = creature.genome.creature_genome.vital;
		const auto &morphology_genome = creature.genome.creature_genome.morphology;
		const auto &body              = creature.body;

		const NormalizedValue<float> remaining =
			1.0f - body.morphology.size.depth.value / morphology_genome.average_back.value;
		const auto muscle_factor = body.vital.life.ratio().value();

		return remaining.value() * muscle_factor;
	}

	float Growing::get_new_height_increment(
		const Creature                                             &creature,
		const Karkinolution::Organisms::Entities::EntitiesRegistry &organisms) {
		const auto &vital_genome      = creature.genome.creature_genome.vital;
		const auto &morphology_genome = creature.genome.creature_genome.morphology;
		const auto &body              = creature.body;

		const NormalizedValue<float> remaining =
			1.0f - body.morphology.size.height.value / morphology_genome.average_back.value;
		const auto muscle_factor = body.vital.life.ratio().value();

		return remaining.value() * muscle_factor;
	}

	CreatureFunction get_diseases_effect(const Creature         &creature,
										 const EntitiesRegistry &entities) {
		return {};
	}

	bool is_dead(const Creature &creature) {
		return creature.body.vital.life.is_zero() || creature.body.metabolism.energy.is_zero();
	}

	bool BrainPhysiology::should_stop_intent(const Brain::Brain &brain) {
		const auto intent = brain.intent();

		if (intent.type == Brain::Intents::IntentTypes::FIND_FOOD) {
			if (intent.time > 7) {
				return true;
			}
			return false;
		} else if (intent.type == Brain::Intents::IntentTypes::NOTHING) {
			if (intent.time > 1) {
				return true;
			}
			return false;
		}
		std::unreachable();
	}

	Creature hatch_embryo(const Karkinolution::Organisms::Entities::Embryo::Embryo &embryo) {
		const Genetic::Genome &genome = embryo.genome;
		const auto            &cg     = genome.creature_genome;
		const auto            &bg     = genome.brain_genome;

		// Gender is determined at hatching, not during gestation.
		const Gender gender = choose_random_gender();

		// The embryo's current energy becomes the neonate's starting energy;
		// the genome-defined max is the ceiling the creature will grow toward.
		MetabolismPart metabolism{
			Energy{embryo.energy.value(), embryo.energy.max()},
			Energy{embryo.energy.value() * 0.5f, cg.metabolism.max_reserved_energy * 0.08f},
			Diet{cg.metabolism.diet},
		};

		VitalPart vital{
			.life     = Life{embryo.life.value(), embryo.life.max()},
			.immunity = cg.vital.average_immunity,
			// Health carries over directly from the embryo's gestational health.
			.health = embryo.health,
		};

		// Reuse the embryo's own size() mapping for dimensional consistency.
		MorphologyPart morphology{
			.size = embryo.size(),
			.mass = Mass{cg.morphology.average_mass.value
						 * (embryo.volume.value
							/ Size::volume(cg.morphology.average_lateral,
										   cg.morphology.average_height,
										   cg.morphology.average_back)
								  .value)},
		};

		Karkinolution::Organisms::Entities::Reproduction::ReproductiveState rep_state{
			gender,
			cg.reproductive.reproductive_way(),
			metabolism.energy,
			vital.health,
		};

		ReproductivePart reproductive{
			.fertility = FertilityCooldown{0, cg.reproductive.fertility_limit},
			.state     = std::move(rep_state),
		};

		Body body{
			.metabolism   = metabolism,
			.vital        = vital,
			.morphology   = morphology,
			.reproduction = std::move(reproductive),
			.age          = Age{0.0f},
		};

		Brain::Brain brain{};
		brain.sociability = bg.average_sociability;

		Ontology ontology{
			.gender = gender,
			.name   = gen_name(),
			.id     = embryo.id,
		};

		return Creature{genome,
						std::move(brain),
						std::move(ontology),
						std::move(body),
						embryo.position};
	}

} // namespace Karkinolution::Organisms::Entities::Creature::Physiology
