#pragma once

#include "shared/math/stats/compile_values.hpp"
#include "karkinolution/organism/entities/creature/actions/presets.hpp"
#include "karkinolution/organism/foods/foods.hpp"
#include "karkinolution/terrain/soil/soil.hpp"

#include <karkinolution/organism/entities/creature/brain/intents.hpp>
#include <karkinolution/organism/entities/creature/brain/perception/perception.hpp>
#include <karkinolution/organism/entities/creature/creature.hpp>
#include <karkinolution/organism/entities/creature/ontology.hpp>
#include <karkinolution/organism/values.hpp>
#include <variant>

namespace Karkinolution::Organisms::Entities::Creature::Brain::Instincts {
	namespace BrainFeelingsCalculator {
		NormalizedValue<float> pregnancy_risK(const Creature &creature);
		NormalizedValue<float> fear(const Creature &creature);
	} // namespace BrainFeelingsCalculator

	namespace ScorerIntents {
		float score_find_food(const Creature &creature);
		float score_nothing(const Creature &creature);
	} // namespace ScorerIntents

	struct ScoredIntent {
			Intents::IntentTypes type;
			float                score;
	};

	namespace Instincts {
		Intents::IntentTypes take(const Creature &creature);
	} // namespace Instincts

	struct FindFoodPresets {
			std::variant<Actions::MovePreset, Actions::EatPreset, std::monostate> value;

			FindFoodPresets(std::variant<Actions::MovePreset, Actions::EatPreset, std::monostate> value)
				: value(value) {}
	};

	struct NothingPresets {
			std::variant<Actions::MovePreset, std::monostate> value;

			NothingPresets(std::variant<Actions::MovePreset, std::monostate> value)
				: value(value) {}
	};

	using AllIntentPresets = std::variant<FindFoodPresets, NothingPresets>;

	namespace PlannerFindFood {
		inline constexpr KShared::Math::Geometry::Radius FIND_FOOD_RADIUS{5};
		inline constexpr double                                MIN_DISTANCE_TO_EAT = 0.5;
		inline constexpr double MIN_SQUARED_DISTANCE_TO_EAT = MIN_DISTANCE_TO_EAT * MIN_DISTANCE_TO_EAT;

		struct Goal {
				Vec2                                                               position;
				FoodHint                                                           hint;
				std::variant<EntityId, Karkinolution::Organisms::Nature::NatureId> id;

				// hint == GRASS -> id = SoilPiecId
				// hint == RAW_MEAT -> id = Id
		};

		std::optional<Goal>
		choose_goal(const Creature                                                                    &creature,
					const Karkinolution::Organisms::Entities::Creature::Brain::Perception::Perception &perception);
		FindFoodPresets
		plan(const Creature                                                                    &creature,
			 const Karkinolution::Organisms::Entities::Creature::Brain::Perception::Perception &perception);
	} // namespace PlannerFindFood

	namespace PlannerNothing {
		NothingPresets
		plan(const Creature                                                                    &creature,
			 const Karkinolution::Organisms::Entities::Creature::Brain::Perception::Perception &perception);
	} // namespace PlannerNothing

	using AllIntentPresets = std::variant<FindFoodPresets, NothingPresets>;

	namespace Planner {
		AllIntentPresets
		resolve_intent(const Creature                                                                    &creature,
					   const Karkinolution::Organisms::Entities::Creature::Brain::Perception::Perception &perception);
		std::optional<Actions::AllPresets>
		plan(const Creature                                                                    &creature,
			 const Karkinolution::Organisms::Entities::Creature::Brain::Perception::Perception &perception);
	} // namespace Planner
} // namespace Karkinolution::Organisms::Entities::Creature::Brain::Instincts
