#pragma once

#include "karkinolution/math/stats/compile_values.hpp"

#include <karkinolution/organism/entities/creature/brain/perception/perception.hpp>
#include <karkinolution/organism/entities/creature/creature.hpp>
#include <karkinolution/organism/entities/creature/ontology.hpp>
#include <karkinolution/organism/foods/foods.hpp>

inline constexpr NormalizedValue<float> FOOD_RATIO_FOR_ENERGY          = 0.5f;
inline constexpr NormalizedValue<float> FOOD_RATIO_FOR_RESERVED_ENERGY = 0.5f;

struct FoodCandidate {
		FoodHint hint;
		Vec3     position;
		Energy   energy;
};

struct FoodDivision {
		float for_energy, for_reserved_energy;
};

namespace MetabolismInstincts {

	NormalizedValue<float> get_diet_preference(const Diet &diet, FoodHint hint);
	NormalizedValue<float> get_diet_weight(const Creature &creature);
	NormalizedValue<float> get_normalized_energy(const Energy &energy, const Creature &creature);
	NormalizedValue<float> preference(const FoodCandidate &food_candidate,
									  const Creature      &creature,
									  const Perception    &perception);
} // namespace MetabolismInstincts

namespace MetabolismPhysiology {
	FoodDivision divide(float energy);
	FoodDivision divide(const Energy &energy);

} // namespace MetabolismPhysiology