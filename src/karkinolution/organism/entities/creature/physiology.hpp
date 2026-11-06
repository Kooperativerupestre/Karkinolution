#pragma once
#include "karkinolution/organism/entities/entities.hpp"

#include <functional>
#include <karkinolution/organism/entities/creature/creature.hpp>
#include <karkinolution/organism/entities/embryo/embryo.hpp>
#include <karkinolution/organism/entities/entities.hpp>
#include <karkinolution/organism/foods/foods.hpp>
#include <karkinolution/organism/foods/nutrient.hpp>

namespace Karkinolution {
	namespace Organisms {
		namespace Entities {
			namespace Creature {
				namespace Physiology {
					struct GrowCost {
							float reserved_energy;
					};

					struct PhysiologyGrowTrade {
							float    gain;
							GrowCost cost;
					};

					using CreatureFunction = std::function<void(Creature &)>;

					namespace Growing {
						PhysiologyGrowTrade get_new_max_energy_increment(const Creature         &creature,
																		 const EntitiesRegistry &organisms);

						PhysiologyGrowTrade get_new_max_energy_reserved_increment(const Creature         &creature,
																				  const EntitiesRegistry &organisms);

						PhysiologyGrowTrade get_new_max_life_increment(const Creature         &creature,
																	   const EntitiesRegistry &organisms);

						float get_new_width_increment(const Creature &creature, const EntitiesRegistry &organisms);

						float get_new_height_increment(const Creature &creature, const EntitiesRegistry &organisms);

					} // namespace Growing

					CreatureFunction get_diseases_effect(const Creature &creature, const EntitiesRegistry &entities);
					bool             is_dead(const Creature &creature);

					// Hatches a mature embryo into a fully-initialised Creature.
					// Maps the embryo's current energy, life, health, area, genome and
					// position directly onto the newborn. Gender is randomly assigned here.
					[[nodiscard]] Creature hatch_embryo(const Embryo::Embryo &embryo);

					namespace BrainPhysiology {
						bool should_stop_intent(const Brain::Brain &brain);
					} // namespace BrainPhysiology

				} // namespace Physiology
			} // namespace Creature
		} // namespace Entities
	} // namespace Organisms
} // namespace Karkinolution
