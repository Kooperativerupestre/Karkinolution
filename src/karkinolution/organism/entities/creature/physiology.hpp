#pragma once
#include "karkinolution/organism/registry.hpp"

#include <karkinolution/organism/entities/creature/creature.hpp>
#include <karkinolution/organism/entities/embryo/embryo.hpp>
#include <karkinolution/organism/entities/entities.hpp>
#include <karkinolution/organism/foods/foods.hpp>
#include <karkinolution/organism/foods/nutrient.hpp>

struct GrowCost {
		float reserved_energy;
};

struct PhysiologyGrowTrade {
		float    gain;
		GrowCost cost;
};

namespace CreatureGrowingPhysiology {
	PhysiologyGrowTrade get_new_max_energy_increment(const Creature         &creature,
													 const OrganismRegistry &organisms);

	PhysiologyGrowTrade get_new_max_energy_reserved_increment(const Creature         &creature,
															  const OrganismRegistry &organisms);

	PhysiologyGrowTrade get_new_max_life_increment(const Creature         &creature,
												   const OrganismRegistry &organisms);

	float get_new_lateral_increment(const Creature &creature, const OrganismRegistry &organisms);

	float get_new_depth_increment(const Creature &creature, const OrganismRegistry &organisms);

	float get_new_height_increment(const Creature &creature, const OrganismRegistry &organisms);

	PhysiologyGrowTrade get_new_muscle_increment(const Creature         &creature,
												 const OrganismRegistry &organisms);

	PhysiologyGrowTrade get_new_skeleton_increment(const Creature         &creature,
												   const OrganismRegistry &organisms);
} // namespace CreatureGrowingPhysiology

namespace CreaturePhysiology {
	CreatureFunction get_to_age_effects(Creature &creature, int age = 1);
	CreatureFunction get_diseases_effect(const Creature         &creature,
										 const EntitiesRegistry &entities);
	bool             is_dead(const Creature &creature);

	// Hatches a mature embryo into a fully-initialised Creature.
	// Maps the embryo's current energy, life, health, volume, genome and
	// position directly onto the newborn. Gender is randomly assigned here.
	[[nodiscard]] Creature hatch_embryo(const Embryo &embryo);
} // namespace CreaturePhysiology

namespace BrainPhysiology {
	bool should_stop_intent(const Brain &brain);
} // namespace BrainPhysiology