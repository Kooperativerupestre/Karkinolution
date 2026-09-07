#include "karkinolution/organism/entities/creature/brain/intents.hpp"
#include "karkinolution/organism/registry.hpp"

#include <karkinolution/math/stats/compile_values.hpp>
#include <karkinolution/organism/entities/creature/creature.hpp>
#include <karkinolution/organism/entities/creature/ontology.hpp>
#include <karkinolution/organism/entities/creature/physiology.hpp>
#include <karkinolution/organism/entities/entities.hpp>
#include <karkinolution/organism/entities/genetics/genetic.hpp>
#include <karkinolution/organism/foods/foods.hpp>
#include <karkinolution/organism/reproduction/state/state.hpp>
#include <karkinolution/organism/stats.hpp>
#include <karkinolution/utils/k_random.hpp>
#include <karkinolution/utils/namegenerator.hpp>
#include <utility>


using Genomes::Resource;

using MetabolismTrait = Genomes::CreatureGenomes::Metabolism::Trait;
using VitalTrait      = Genomes::CreatureGenomes::Vital::Trait;
using MuscleTrait     = Genomes::CreatureGenomes::MuscleStructure::Trait;
using SkeletonTrait   = Genomes::CreatureGenomes::SkeletonStructure::Trait;

// ============================================================================
// CreatureGrowingPhysiology
// ============================================================================

PhysiologyGrowTrade
CreatureGrowingPhysiology::get_new_max_energy_increment(const Creature         &creature,
														const OrganismRegistry &organism) {
	const auto &metabolism_genome = creature.genome.creature_genome.metabolism;
	const auto &metabolism        = creature.body.metabolism;

	const float max_energy = metabolism_genome.max_energy;

	const NormalizedValue<float> remaining = 1.0f - metabolism.energy.max() / max_energy;

	const auto &conversion =
		metabolism_genome.transformations.at(Resource::RESERVED_ENERGY, MetabolismTrait::MAX_ENERGY)
			.efficiency;

	const GrowthRate growth = metabolism_genome.growth_rates.max_energy;

	const float gain = remaining.value() * max_energy * growth.value;

	const float reserved_energy_cost = gain / conversion.value();

	return PhysiologyGrowTrade{.gain = gain,
							   .cost = GrowCost{.reserved_energy = reserved_energy_cost}};
}

PhysiologyGrowTrade CreatureGrowingPhysiology::get_new_max_energy_reserved_increment(
	const Creature         &creature,
	const OrganismRegistry &organisms) {
	const auto &metabolism_genome = creature.genome.creature_genome.metabolism;
	const auto &metabolism        = creature.body.metabolism;

	const float max_reserved_energy = metabolism_genome.max_reserved_energy;

	const NormalizedValue<float> remaining = 1.0f - metabolism.reserved.max() / max_reserved_energy;

	const auto &conversion =
		metabolism_genome.transformations
			.at(Resource::RESERVED_ENERGY, MetabolismTrait::MAX_RESERVED_ENERGY)
			.efficiency;

	const GrowthRate growth = metabolism_genome.growth_rates.max_reserved_energy;

	const float gain = remaining.value() * max_reserved_energy * growth.value;

	const float reserved_energy_cost = gain / conversion.value();

	return PhysiologyGrowTrade{.gain = gain,
							   .cost = GrowCost{.reserved_energy = reserved_energy_cost}};
}

PhysiologyGrowTrade
CreatureGrowingPhysiology::get_new_max_life_increment(const Creature         &creature,
													  const OrganismRegistry &organisms) {
	const auto &vital_genome = creature.genome.creature_genome.vital;
	const auto &body         = creature.body;

	const float max_life = vital_genome.max_life;

	const NormalizedValue<float> remaining = 1.0f - body.vital.life.max() / max_life;

	const auto &conversion =
		vital_genome.transformations.at(Resource::RESERVED_ENERGY, VitalTrait::MAX_LIFE).efficiency;

	const GrowthRate growth = vital_genome.growth_rates.life.value;

	const NormalizedValue<float> nutrition = 0.2f + body.metabolism.reserved.ratio();

	const float gain = remaining.value() * max_life * growth.value * nutrition.value();

	const float reserved_energy_cost = gain / conversion.value();

	return PhysiologyGrowTrade{.gain = gain,
							   .cost = GrowCost{.reserved_energy = reserved_energy_cost}};
}

// TODO
// Integrate MorbusExiguus infection

float CreatureGrowingPhysiology::get_new_lateral_increment(const Creature         &creature,
														   const OrganismRegistry &organisms) {
	const auto &vital_genome      = creature.genome.creature_genome.vital;
	const auto &morphology_genome = creature.genome.creature_genome.morphology;
	const auto &body              = creature.body;

	const NormalizedValue<float> remaining =
		1.0f - body.morphology.size.lateral.value / morphology_genome.average_lateral.value;
	const Factor<float, 2.0f, 0.0f> muscle_factor{creature.specie_relative_muscle()};

	return remaining.value() * muscle_factor.value();
}

float CreatureGrowingPhysiology::get_new_depth_increment(const Creature         &creature,
														 const OrganismRegistry &organisms) {
	const auto &vital_genome      = creature.genome.creature_genome.vital;
	const auto &morphology_genome = creature.genome.creature_genome.morphology;
	const auto &body              = creature.body;

	const NormalizedValue<float> remaining =
		1.0f - body.morphology.size.back.value / morphology_genome.average_back.value;
	const Factor<float, 2.0f, 0.0f> muscle_factor{creature.specie_relative_muscle()};

	return remaining.value() * muscle_factor.value();
}

float CreatureGrowingPhysiology::get_new_height_increment(const Creature         &creature,
														  const OrganismRegistry &organisms) {
	const auto &vital_genome      = creature.genome.creature_genome.vital;
	const auto &morphology_genome = creature.genome.creature_genome.morphology;
	const auto &body              = creature.body;

	const NormalizedValue<float> remaining =
		1.0f - body.morphology.size.height.value / morphology_genome.average_back.value;
	const Factor<float, 2.0f, 0.0f> muscle_factor{creature.specie_relative_muscle()};

	return remaining.value() * muscle_factor.value();
}

PhysiologyGrowTrade
CreatureGrowingPhysiology::get_new_muscle_increment(const Creature         &creature,
													const OrganismRegistry &organism) {
	const auto &muscle_genome = creature.genome.creature_genome.muscle;

	const auto &body = creature.body;

	const NormalizedValue<float> muscle_factor = creature.specie_relative_muscle();

	const float efficiency =
		muscle_genome.transformations.at(Resource::RESERVED_ENERGY, MuscleTrait::MUSCLE)
			.efficiency.value();

	const float gain = muscle_factor.value() * body.metabolism.reserved.ratio();

	const float reserved_energy_cost = gain / efficiency;

	return PhysiologyGrowTrade{.gain = gain,
							   .cost = GrowCost{.reserved_energy = reserved_energy_cost}};
}

PhysiologyGrowTrade
CreatureGrowingPhysiology::get_new_skeleton_increment(const Creature         &creature,
													  const OrganismRegistry &organisms) {
	const auto &skeleton_genome = creature.genome.creature_genome.skeleton;

	const auto &body = creature.body;

	const NormalizedValue<float> skeleton_factor = creature.specie_relative_bone();

	const float efficiency =
		skeleton_genome.transformations.at(Resource::RESERVED_ENERGY, SkeletonTrait::BONES)
			.efficiency.value();

	const float gain = skeleton_factor.value() * body.metabolism.reserved.ratio();

	const float reserved_energy_cost = gain / efficiency;

	return PhysiologyGrowTrade{.gain = gain,
							   .cost = GrowCost{.reserved_energy = reserved_energy_cost}};
}

CreatureFunction CreaturePhysiology::get_to_age_effects(Creature &creature, int age) {
	return [](Creature &creature) {
		creature.body.age.value++;
	};
}

CreatureFunction CreaturePhysiology::get_diseases_effect(const Creature         &creature,
														 const EntitiesRegistry &entities) {
	return {};
}

bool BrainPhysiology::should_stop_intent(const Brain &brain) {
	const auto intent = brain.intent();

	if (intent.type == IntentTypes::FIND_FOOD) {
		if (intent.time > 7) {
			return true;
		}
		return false;
	} else if (intent.type == IntentTypes::NOTHING) {
		if (intent.time > 1) {
			return true;
		}
		return false;
	}
	std::unreachable();
}

Creature CreaturePhysiology::hatch_embryo(const Embryo &embryo) {
	const Genome &genome = embryo.genome;
	const auto   &cg     = genome.creature_genome;
	const auto   &bg     = genome.brain_genome;

	// Gender is determined at hatching, not during gestation.
	const Gender gender = GenderF::choice();

	// The embryo's current energy becomes the neonate's starting energy;
	// the genome-defined max is the ceiling the creature will grow toward.
	Metabolism metabolism{
		Energy{embryo.energy.value(), embryo.energy.max()},
		Energy{embryo.energy.value() * 0.5f, cg.metabolism.max_reserved_energy * 0.08f},
		Diet{cg.metabolism.diet},
	};

	Vital vital{
		.life     = Life{embryo.life.value(), embryo.life.max()},
		.immunity = cg.vital.average_immunity,
		// Health carries over directly from the embryo's gestational health.
		.health = embryo.health,
	};

	// Reuse the embryo's own size() mapping for dimensional consistency.
	Morphology morphology{
		.size = embryo.size(),
		.mass = Mass{cg.morphology.average_mass.value
					 * (embryo.volume.value
						/ Size::volume(cg.morphology.average_lateral,
									   cg.morphology.average_height,
									   cg.morphology.average_back)
							  .value)},
	};

	SkeletonStructure skeleton{
		.shared_volume = cg.skeleton.shared_volume,
		.bone          = Bone{cg.skeleton.average_bones.value * 0.08f},
		.quality       = cg.skeleton.average_bone_quality,
	};

	MuscleStructure muscle{
		.muscle        = Muscle{cg.muscle.average_muscles.value * 0.08f},
		.efficiency    = cg.muscle.average_muscle_efficiency,
		.quality       = cg.muscle.average_muscle_quality,
		.shared_volume = cg.muscle.shared_volume,
	};

	ReproductiveState rep_state{
		gender,
		cg.reproductive.reproductive_way(),
		metabolism.energy,
		vital.health,
	};

	BodyReproductive reproductive{
		.fertility = FertilityCooldown{0, cg.reproductive.fertility_limit},
		.state     = std::move(rep_state),
	};

	Body body{
		.metabolism   = std::move(metabolism),
		.vital        = std::move(vital),
		.reproductive = std::move(reproductive),
		.morphology   = std::move(morphology),
		.skeleton     = std::move(skeleton),
		.muscle       = std::move(muscle),
		.age          = Age{0.0f},
	};

	Brain brain{};
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