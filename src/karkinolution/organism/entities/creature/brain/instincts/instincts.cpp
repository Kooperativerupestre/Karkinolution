#include "karkinolution/math/physic/vec/model.hpp"
#include "karkinolution/math/stats/compile_values.hpp"
#include "karkinolution/organism/entities/creature/actions/presets.hpp"
#include "karkinolution/organism/reproduction/state/validator.hpp"

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

FindFoodPresets PlannerFindFood::plan(const Creature &creature, const Perception &perception) {
	const auto view = PerceptionAnalyzer::reduce(perception, GeometryForms::Radius{1.5});

	return FindFoodPresets{MovePreset{.new_coord = view.resolved_soils().back().get().position}};
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

std::variant<MovePreset, std::monostate> Planner::plan(const Creature   &creature,
													   const Perception &perception) {
	const auto output = resolve_intent(creature, perception);

	if (std::holds_alternative<FindFoodPresets>(output)) {
		return std::get<FindFoodPresets>(output).value;
	} else if (std::holds_alternative<NothingPresets>(output)) {
		return std::get<NothingPresets>(output).value;
	}
	std::unreachable();
}