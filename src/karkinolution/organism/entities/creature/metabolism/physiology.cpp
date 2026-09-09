
#include <karkinolution/organism/entities/creature/creature.hpp>
#include <karkinolution/organism/entities/creature/metabolism/physiology.hpp>
#include <karkinolution/organism/entities/creature/ontology.hpp>
#include <karkinolution/organism/foods/foods.hpp>

FoodDivision MetabolismPhysiology::divide(float energy) {
	return FoodDivision{.for_energy          = energy * FOOD_RATIO_FOR_ENERGY,
						.for_reserved_energy = energy * FOOD_RATIO_FOR_RESERVED_ENERGY};
}

FoodDivision MetabolismPhysiology::divide(const Energy &energy) {
	return divide(energy.value());
}