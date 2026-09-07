#pragma once

#include "karkinolution/math/stats/compile_values.hpp"
#include "karkinolution/organism/entities/creature/actions/presets.hpp"

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
		std::variant<MovePreset, std::monostate> value;

		FindFoodPresets(std::variant<MovePreset, std::monostate> value)
			: value(value) {}
};

struct NothingPresets {
		std::variant<MovePreset, std::monostate> value;

		NothingPresets(std::variant<MovePreset, std::monostate> value)
			: value(value) {}
};

using AllIntentPresets = std::variant<FindFoodPresets, NothingPresets>;

namespace PlannerFindFood {
	FindFoodPresets plan(const Creature &creature, const Perception &perception);
} // namespace PlannerFindFood

namespace PlannerNothing {
	NothingPresets plan(const Creature &creature, const Perception &perception);
} // namespace PlannerNothing

using AllIntentPresets = std::variant<FindFoodPresets, NothingPresets>;
using PlannerOutput    = std::variant<MovePreset, std::monostate>;

namespace Planner {
	AllIntentPresets resolve_intent(const Creature &creature, const Perception &perception);
	PlannerOutput    plan(const Creature &creature, const Perception &perception);
} // namespace Planner