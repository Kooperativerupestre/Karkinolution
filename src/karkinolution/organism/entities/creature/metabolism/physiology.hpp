#pragma once

#include "karkinolution/math/stats/compile_values.hpp"

#include <karkinolution/organism/entities/creature/brain/perception/perception.hpp>
#include <karkinolution/organism/entities/creature/creature.hpp>
#include <karkinolution/organism/entities/creature/ontology.hpp>
#include <karkinolution/organism/foods/foods.hpp>

inline constexpr NormalizedValue<float> FOOD_RATIO_FOR_ENERGY          = 0.5f;
inline constexpr NormalizedValue<float> FOOD_RATIO_FOR_RESERVED_ENERGY = 0.5f;

struct FoodDivision {
		float for_energy, for_reserved_energy;
};

namespace MetabolismPhysiology {
	FoodDivision divide(float energy);
	FoodDivision divide(const Energy &energy);
	FoodDivision divide(const GrassMatter &grass);

} // namespace MetabolismPhysiology