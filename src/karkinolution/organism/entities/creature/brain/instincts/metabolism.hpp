#pragma once
#include "karkinolution/math/stats/compile_values.hpp"

#include <karkinolution/organism/entities/creature/brain/perception/perception.hpp>
#include <karkinolution/organism/entities/creature/creature.hpp>
#include <karkinolution/organism/entities/creature/ontology.hpp>
#include <karkinolution/organism/foods/foods.hpp>

struct FoodCandidate {
		FoodHint hint;
		Vec3     position;
		float    energy;
};

namespace MetabolismInstincts {


	FoodCandidate                make_corpse_canditate(const PerceivedCorpse &corpse);
	std::optional<FoodCandidate> make_soil_candidate(const PerceivedSoil &soil);

	NormalizedValue<float> get_diet_preference(const Diet &diet, FoodHint hint);
	NormalizedValue<float> get_diet_weight(const Creature &creature);
	NormalizedValue<float> get_normalized_energy(float energy, const Creature &creature);
	NormalizedValue<float> preference(const FoodCandidate &food_candidate,
									  const Creature      &creature,
									  const Perception    &perception);
} // namespace MetabolismInstincts