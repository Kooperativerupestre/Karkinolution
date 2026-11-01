
#include <karkinolution/organism/entities/creature/creature.hpp>
#include <karkinolution/organism/entities/creature/metabolism/physiology.hpp>
#include <karkinolution/organism/entities/creature/ontology.hpp>
#include <karkinolution/organism/foods/foods.hpp>

namespace Karkinolution::Organisms::Entities::Creature::Metabolism::Physiology {

FoodDivision divide(float energy) {
	return FoodDivision{.for_energy          = energy * FOOD_RATIO_FOR_ENERGY.value(),
						.for_reserved_energy = energy * FOOD_RATIO_FOR_RESERVED_ENERGY.value()};
}

FoodDivision divide(const Karkinolution::Organisms::Energy &energy) {
	return divide(energy.value());
}

FoodDivision divide(const Karkinolution::Organisms::Nature::Grass::GrassMatter &grass) {
	return divide(grass.value());
}

} // namespace Karkinolution::Organisms::Entities::Creature::Metabolism::Physiology
