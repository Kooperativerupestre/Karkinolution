#include "karkinolution/organism/registry.hpp"

#include <karkinolution/math/stats/compile_values.hpp>
#include <karkinolution/organism/entities/creature/creature.hpp>
#include <karkinolution/organism/entities/creature/physiology.hpp>
#include <karkinolution/organism/entities/entities.hpp>
#include <karkinolution/organism/entities/genetics/genetic.hpp>
#include <karkinolution/organism/foods/foods.hpp>
#include <karkinolution/organism/stats.hpp>


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

// ============================================================================
// CreatureMetabolismPhysiology
// ============================================================================

Trade CreatureMetabolismPhysiology::metabolize(const Creature         &creature,
											   const RawMeat          &raw_meat,
											   const EntitiesRegistry &entities) {
	return {};
}

// ============================================================================
// CreaturePhysiology
// ============================================================================

CreatureFunction CreaturePhysiology::get_to_age_effects(Creature &creature, int age) {
	return [](Creature &creature) {
		creature.body.age.value++;
	};
}

CreatureFunction CreaturePhysiology::get_diseases_effect(const Creature         &creature,
														 const EntitiesRegistry &entities) {
	return {};
}