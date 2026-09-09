#pragma once

#include "karkinolution/math/stats/compile_values.hpp"
#include "karkinolution/organism/entities/creature/actions/presets.hpp"
#include "karkinolution/organism/foods/foods.hpp"
#include "karkinolution/terrain/soil.hpp"

#include <karkinolution/organism/entities/creature/brain/intents.hpp>
#include <karkinolution/organism/entities/creature/brain/perception/perception.hpp>
#include <karkinolution/organism/entities/creature/creature.hpp>
#include <karkinolution/organism/entities/creature/ontology.hpp>
#include <karkinolution/organism/stats.hpp>
#include <variant>

namespace BrainFeelingsCalculator {
	NormalizedValue<float> pregnancy_risK(const Creature &creature);
	NormalizedValue<float> fear(const Creature &creature);
} // namespace BrainFeelingsCalculator

namespace ScorerIntents {
	float score_find_food(const Creature &creature);
	float score_nothing(const Creature &creature);
} // namespace ScorerIntents

struct ScoredIntent {
		IntentTypes type;
		float       score;
};

namespace Instincts {
	IntentTypes take(const Creature &creature);
} // namespace Instincts

struct FindFoodPresets {
		std::variant<MovePreset, EatPreset, std::monostate> value;

		FindFoodPresets(std::variant<MovePreset, EatPreset, std::monostate> value)
			: value(value) {}
};

struct NothingPresets {
		std::variant<MovePreset, std::monostate> value;

		NothingPresets(std::variant<MovePreset, std::monostate> value)
			: value(value) {}
};

using AllIntentPresets = std::variant<FindFoodPresets, NothingPresets>;

namespace PlannerFindFood {
	inline constexpr Radius FIND_FOOD_RADIUS{5};
	inline constexpr double MIN_DISTANCE_TO_EAT         = 0.5;
	inline constexpr double MIN_SQUARED_DISTANCE_TO_EAT = MIN_DISTANCE_TO_EAT * MIN_DISTANCE_TO_EAT;

	struct Goal {
			Vec3                          position;
			FoodHint                      hint;
			std::variant<Id, SoilPieceId> id;

			// hint == GRASS -> id = SoilPiecId
			// hint == RAW_MEAT -> id = Id
	};

	std::optional<Goal> choose_goal(const Creature &creature, const Perception &perception);
	FindFoodPresets     plan(const Creature &creature, const Perception &perception);
} // namespace PlannerFindFood

namespace PlannerNothing {
	NothingPresets plan(const Creature &creature, const Perception &perception);
} // namespace PlannerNothing

using AllIntentPresets = std::variant<FindFoodPresets, NothingPresets>;

namespace Planner {
	AllIntentPresets resolve_intent(const Creature &creature, const Perception &perception);
	std::optional<AllPresets> plan(const Creature &creature, const Perception &perception);
} // namespace Planner