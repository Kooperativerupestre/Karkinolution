#include "karkinolution/core/error.hpp"
#include "karkinolution/math/physic/vec/model.hpp"
#include "karkinolution/math/stats/compile_values.hpp"
#include "karkinolution/organism/entities/creature/actions/presets.hpp"
#include "karkinolution/organism/entities/creature/brain/instincts/metabolism.hpp"
#include "karkinolution/organism/reproduction/state/validator.hpp"
#include "karkinolution/terrain/soil.hpp"

#include <karkinolution/organism/entities/creature/brain/instincts/instincts.hpp>
#include <karkinolution/organism/entities/creature/brain/intents.hpp>
#include <karkinolution/organism/entities/creature/brain/perception/perception.hpp>
#include <karkinolution/organism/entities/creature/creature.hpp>
#include <karkinolution/organism/entities/creature/ontology.hpp>
#include <karkinolution/organism/stats.hpp>
#include <karkinolution/utils/k_random.hpp>
#include <utility>
#include <variant>

NormalizedValue<float> BrainFeelingsCalculator::pregnancy_risK(const Creature &creature) {
	NormalizedValue<float> factor;
	if (!creature.body.reproductive.state.is_pregnant()) {
		throw EntityError("Only pregnant creature have risk factor");
	}

	ReproductionValidator::is_pregnant(creature.body.reproductive.state);

	const auto uterus = std::get<Uterus>(creature.body.reproductive.state.state);

	factor += uterus.get_pregnant_uterus().born_count.ratio();
	return factor;
}

NormalizedValue<float> BrainFeelingsCalculator::fear(const Creature &creature) {
	NormalizedValue<float> factor;

	const auto life_ratio = creature.body.vital.life.ratio();

	if (life_ratio.value() < 0.3f) {
		factor += life_ratio - 0.05;
	}
	const auto &pregnant_uterus =
		std::get<Uterus>(creature.body.reproductive.state.state).get_pregnant_uterus();

	if (creature.body.reproductive.state.is_pregnant()) {
		factor += pregnant_uterus.gestation.ratio() * 0.5;
	}
	factor += creature.specie_relative_age() * 0.4;
	return factor;
}

float ScorerIntents::score_find_food(const Creature &creature) {
	NormalizedValue<float> factor;

	const auto hungry = creature.body.hungry();

	if (hungry.value() > 0.75f) {
		factor += hungry * 1.3;
	} else {
		factor += hungry * 1.12;
	}

	if (creature.body.reproductive.state.is_pregnant()) {
		factor += std::get<Uterus>(creature.body.reproductive.state.state)
					  .get_pregnant_uterus()
					  .gestation.ratio()
			* 0.4;
	}
	return factor;
}

float ScorerIntents::score_nothing(const Creature &creature) {
	const float base = 0.6;

	NormalizedValue<float> factor{base};

	factor += creature.specie_relative_age() * 0.25f;
	return factor.value();
}

IntentTypes Instincts::take(const Creature &creature) {
	std::array<ScoredIntent, 2> scored_intents;

	// 1 -> Nothing
	// 2 -> Find Food

	scored_intents[1] =
		ScoredIntent{.type = IntentTypes::NOTHING, .score = ScorerIntents::score_nothing(creature)};
	scored_intents[2] = ScoredIntent{.type  = IntentTypes::FIND_FOOD,
									 .score = ScorerIntents::score_find_food(creature)};


	ScoredIntent best_s_i{.type  = IntentTypes::NOTHING,
						  .score = std::numeric_limits<float>().lowest()};
	for (auto s_i : scored_intents) {
		if (s_i.score > best_s_i.score) {
			best_s_i = s_i;
		}
	}
	return best_s_i.type;
}

std::optional<PlannerFindFood::Goal> PlannerFindFood::choose_goal(const Creature   &creature,
																  const Perception &perception) {
	const auto output = PerceptionAnalyzer::reduce(perception, FIND_FOOD_RADIUS);

	FoodCandidate*                               food_canditate = nullptr;
	NormalizedValue<float>                       best_score{0.0};
	std::optional<std::variant<Id, SoilPieceId>> id = std::nullopt;

	for (const auto &_entity : output.resolved_entities()) {
		const auto &entity = _entity.get();
		if (std::holds_alternative<PerceivedCorpse>(entity)) {
			const auto   &corpse      = std::get<PerceivedCorpse>(entity);
			FoodCandidate current_f_c = MetabolismInstincts::make_corpse_canditate(corpse);

			const auto current_score =
				MetabolismInstincts::preference(current_f_c, creature, perception);


			if (food_canditate == nullptr || best_score < current_score) {
				food_canditate = &current_f_c;
				best_score     = current_score;
				id             = corpse.id;
			}
		}
	}

	for (const auto &_soil : output.resolved_soils()) {
		const auto &soil = _soil.get();

		std::optional<FoodCandidate> current_f_c = MetabolismInstincts::make_soil_candidate(soil);
		if (current_f_c.has_value()) {
			const auto current_score =
				MetabolismInstincts::preference(current_f_c.value(), creature, perception);


			if (food_canditate == nullptr || best_score < current_score) {
				food_canditate = &current_f_c.value();
				best_score     = current_score;
				id             = soil.id;
			}
		}
	}

	if (food_canditate == nullptr) {
		throw SimulationError("Food candidate cannot be null");
	}

	if (!id.has_value()) {
		return std::nullopt;
	}

	return Goal{.position = food_canditate->position,
				.hint     = food_canditate->hint,
				.id       = id.value()};
}

FindFoodPresets PlannerFindFood::plan(const Creature &creature, const Perception &perception) {
	const auto goal_ = choose_goal(creature, perception);

	if (!goal_.has_value()) {
		return FindFoodPresets{std::monostate()};
	}
	const auto &goal = goal_.value();

	if (goal.position.distance_squared_to(creature.position) > MIN_SQUARED_DISTANCE_TO_EAT) {
		return FindFoodPresets{MovePreset{.new_coord = goal.position}};
	}
	return FindFoodPresets{EatPreset{.hint = goal.hint, .id = goal.id}};
}

NothingPresets PlannerNothing::plan(const Creature &creature, const Perception &perception) {
	bool const want_to_move = Choices::choice_bool(0.5, 0.5);

	if (want_to_move) {
		const Vec3 new_coord = creature.position + Vec3{1.0, 0.0, 0.0};
		return NothingPresets{MovePreset{.new_coord = new_coord}};
	}
	return NothingPresets{std::monostate()};
}

std::variant<FindFoodPresets, NothingPresets>
Planner::resolve_intent(const Creature &creature, const Perception &perception) {
	if (creature.brain.intent().type == IntentTypes::NOTHING) {
		return PlannerNothing::plan(creature, perception);
	} else if (creature.brain.intent().type == IntentTypes::FIND_FOOD) {
		return PlannerFindFood::plan(creature, perception);
	}

	std::unreachable();
}

std::optional<AllPresets> Planner::plan(const Creature &creature, const Perception &perception) {
	const auto output = resolve_intent(creature, perception);

	if (std::holds_alternative<FindFoodPresets>(output)) {
		const auto &preset = std::get<FindFoodPresets>(output);

		if (std::holds_alternative<EatPreset>(preset.value)) {
			return std::get<EatPreset>(preset.value);
		} else if (std::holds_alternative<MovePreset>(preset.value)) {
			return std::get<MovePreset>(preset.value);
		} else if (std::holds_alternative<std::monostate>(preset.value)) {
			return std::nullopt;
		}
		std::unreachable();
	} else if (std::holds_alternative<NothingPresets>(output)) {
		const auto &preset = std::get<NothingPresets>(output);

		if (std::holds_alternative<MovePreset>(preset.value)) {
			return std::get<MovePreset>(preset.value);
		} else if (std::holds_alternative<std::monostate>(preset.value)) {
			return std::nullopt;
		}
		std::unreachable();
	}
	std::unreachable();
}