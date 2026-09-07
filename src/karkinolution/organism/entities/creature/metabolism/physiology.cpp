#include "karkinolution/math/stats/compile_values.hpp"
#include "karkinolution/organism/entities/creature/brain/perception/perception.hpp"

#include <karkinolution/organism/entities/creature/creature.hpp>
#include <karkinolution/organism/entities/creature/metabolism/physiology.hpp>
#include <karkinolution/organism/entities/creature/ontology.hpp>
#include <karkinolution/organism/foods/foods.hpp>
#include <utility>

NormalizedValue<float> MetabolismInstincts::get_diet_preference(const Diet &diet, FoodHint hint) {
	if (hint == FoodHint::GRASS) {
		return diet.grass_score();
	} else if (hint == FoodHint::RAW_MEAT) {
		return diet.raw_meat_score();
	}
	std::unreachable();
}

NormalizedValue<float> MetabolismInstincts::get_diet_weight(const Creature &creature) {
	return 1.0f - creature.body.hungry();
}

NormalizedValue<float> MetabolismInstincts::get_normalized_energy(const Energy   &energy,
																  const Creature &creature) {
	return energy.value()
		/ (creature.body.metabolism.energy.max() * 0.8
		   + creature.body.metabolism.reserved.max() * 1.3);
}

NormalizedValue<float> MetabolismInstincts::preference(const FoodCandidate &food_candidate,
													   const Creature      &creature,
													   const Perception    &perception) {
	const auto &diet             = creature.body.metabolism.diet;
	const float diet_prefference = get_diet_preference(diet, food_candidate.hint);

	auto diet_weight = get_diet_weight(creature);

	auto distance = PerceptionAnalyzer::normalize_distance(perception, food_candidate.position);

	const NormalizedValue<float> distance_normalized{static_cast<float>(distance.value()) * 0.75f};
	const auto                   diet_force = diet_weight * diet_prefference * 0.25f;
	const auto energy_normalized = get_normalized_energy(food_candidate.energy, creature);

	// 0.75 + 0.25 + 1 = 2


	return (distance_normalized + diet_force + energy_normalized) / 2.0f;
}

FoodDivision MetabolismPhysiology::divide(float energy) {
	return FoodDivision{.for_energy          = energy * FOOD_RATIO_FOR_ENERGY,
						.for_reserved_energy = energy * FOOD_RATIO_FOR_RESERVED_ENERGY};
}

FoodDivision MetabolismPhysiology::divide(const Energy &energy) {
	return divide(energy.value());
}