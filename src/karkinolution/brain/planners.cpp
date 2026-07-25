
#include <karkinolution/brain/instincts/factors.hpp>
#include <karkinolution/terrain/map.hpp>
#include <karkinolution/systems/physics.hpp>
#include <karkinolution/brain/planners.hpp>
#include <karkinolution/organism/ontology.hpp>
#include <karkinolution/organism/creatures.hpp>
#include <karkinolution/brain/instincts/instincts.hpp>
#include <karkinolution/core/vec2.hpp>
#include <karkinolution/core/stats.hpp>
#include <karkinolution/actions/presets.hpp>
#include <karkinolution/utils/dataclasses.hpp>
#include <karkinolution/systems/metabolism.hpp>
#include <variant>
#include <karkinolution/brain/perception.hpp>
#include <optional>
#include <cmath>
#include <karkinolution/utils/k_random.hpp>
#include <karkinolution/core/global_epsilon.hpp>
#include <karkinolution/systems/reproduction.hpp>

bool PlannerFindFood::can_eat_food_option(const FoodOption& food_option, const Creature& creature) {
    float diet_bias = creature.genome.metabolism.diet[food_option.hint];
    float diet_effective = MetabolismSystem::diet_effective(diet_bias, creature.hungry());
    return diet_effective >= MIN_DIET_PREFERENCE_EFFECTIVENESS;
}

SignedNormalizedValue PlannerFindFood::score_food_option(
    const FoodOption& food_option,
    const Creature& creature,
    const DangerIndex& danger_index,
    const Perception& perception
) {
    float movement_cost = MovementSystem::calculate_cost_distance_to_move(perception.at(food_option.coord).distance, creature);
    float cost = std::pow(movement_cost, 1.2);
    float diet_bias = creature.genome.metabolism.diet[food_option.hint];

    if (food_option.hint == FoodHint::TARGET) {
        cost += (AttackSystem::calculate_cost_to_kill(creature, perception.at(food_option.coord).get_creature()) *
         Disturbs::gen_disturb(1 - NOISY_HITS_TO_KILL, 1 + NOISY_HITS_TO_KILL));
    }

    NormalizedValue normalized_cost;
    NormalizedValue normalized_gain;

    normalized_cost.set(cost / creature.energy.max());
    normalized_gain.set(food_option.energy_gain / creature.energy.max());

    float food_urge = MetabolismSystem::diet_effective(diet_bias, creature.hungry()) * normalized_gain.value();
    SignedNormalizedValue value{food_urge - normalized_cost.value()};

    auto dangers = danger_index.try_at(food_option.coord);
    
    if (dangers == nullptr) {
        value -= 0.1;
    } else {
        value -= dangers->accumulated_danger();
    }

    return value;
}

std::optional<FoodCoord> PlannerFindFood::choose_best_food_coord(
    const Perception& perception,
    const DangerIndex& danger_index,
    const Creature& creature
) {
    std::optional<FoodCoord> best = std::nullopt;
    SignedNormalizedValue best_score = {-1.0};

    for (const auto& [c, unused] : perception.pieces_ref()) {
        std::vector<FoodOption> food_options = MetabolismSystem::get_food_options(perception, c);

        for (const auto& f_o : food_options) {
            SignedNormalizedValue score = PlannerFindFood::score_food_option(f_o, creature, danger_index, perception);

            if (!best.has_value() || score > best_score) {
                best_score = score;
                best.emplace(FoodCoord{
                    .coord = c,
                    .hint = f_o.hint,
                    .score = score
                });
            }
        }
    }

    return best;
}

FindFoodPresetsOutput PlannerFindFood::decide_preset(const FoodCoord& food_coord, const Perception& perception, EntitiesRegistry& entities) {
    auto block = perception.at(food_coord.coord);

    if (block.distance <= Approx<float>(1) && food_coord.hint == FoodHint::GRASS) {
        return EatPreset{.energy = block.cell.food.value(), .food_hint = food_coord.hint};
    }

    if (block.distance == Approx<float>(1)) {
        if (food_coord.hint == FoodHint::CORPSE) {
            return EatPreset{.energy = entities.at_corpse(block.get_corpse().id).energy, .food_hint = food_coord.hint};
        } else if (food_coord.hint == FoodHint::TARGET) {
            return AttackPreset{.target = block.get_creature().id};
        }
    }
    return MovePreset{food_coord.coord};
}

std::optional<FindFoodPresetsOutput> PlannerFindFood::plan_intent(
    const Perception& perception,
    const DangerIndex& danger_index,
    const Creature& creature,
    EntitiesRegistry& entities
) {
    auto chosen = PlannerFindFood::choose_best_food_coord(perception, danger_index, creature);
    if (!chosen.has_value()) {
        return std::nullopt;
    }
    return PlannerFindFood::decide_preset(*chosen, perception, entities);
}

std::optional<MovePreset> PlannerFindMatch::plan_intent(const Perception& perception, const Creature& creature) {
    auto possibliy_matches = PerceptionPatterns::find_same_species(perception,
         [&creature](const PerceivedBlock& block, Vec2 coord) { return block.has_creature() && ReproductiveSystem::can_reproduce(creature, block.get_creature()); });
    if (possibliy_matches.size() == 0) {
        return std::nullopt;
    }

    Vec2 near_coord = PerceptionAnalyser::near_coord(possibliy_matches);

    if (Vec2F::distance_exceeds_one(perception.coord(), near_coord)) {
        return MovePreset{near_coord};
    }
    return std::nullopt;
}

std::optional<AttackPresetsOutput> PlannerAttack::plan_intent(
    const AttackOutput& attack_input,
    const Perception& perception,
    const EntitiesRegistry& entities
) {
    Vec2 target_coord = entities.at_creature(attack_input.target_id).position;

    if (Vec2F::distance_exceeds_one(perception.coord(), target_coord)) {
        return MovePreset{target_coord};
    }
    return AttackPreset(attack_input.target_id);
}

NormalizedValue PlannerNothing::score_coord(const Perception& perception, Vec2 coord, const Creature& creature) {
    const PerceivedBlock& block = perception.at(coord);
    
    bool has_creature = block.has_creature();
    bool has_same_specie = has_creature && BlockProperties::is_same_specie(block.get_creature(), perception.creature());
    NormalizedValue distance = block.distance / perception.max_distance();

    float affinity = has_same_specie ? 1 : (has_creature ? 0.4 : 0);
    NormalizedValue factors = Factors::get_territorial_indifference_factor(creature) * Factors::get_pregnant_social_interest_factor(creature);
    return affinity * factors.value() * creature.sociability.value() - distance.value();
}

std::optional<MovePreset> PlannerNothing::plan_intent(const Perception& perception, const Creature& creature) {
    bool chosen = Choices::choice_bool(1 - creature.hungry().value(), creature.hungry().value());

    if (!chosen) {
        return std::nullopt;
    }

    auto new_perception = PerceptionAnalyser::get_area_in_radius_ratio(perception, 0.7);

    std::optional<ScoredCoord> scored_coord = std::nullopt;

    for (const auto& [c, b] : new_perception.pieces_ref()) {
        NormalizedValue score = PlannerNothing::score_coord(perception, c, creature);

        if (scored_coord == std::nullopt) {
            scored_coord.emplace(c, score);
        } else if (scored_coord->score < score) {
            scored_coord.emplace(c, score);
        }
    }
    return MovePreset{scored_coord->coord};
}

std::optional<std::variant<MovePreset, EatPreset, AttackPreset>> Planner::resolve_intent(
    const Perception& perception,
    const Creature& creature,
    const DangerIndex& danger_index,
    EntitiesRegistry&entities
) {
    IntentActs act = creature.intent.act;

    if (act == IntentActs::FIND_FOOD) {
        return PlannerFindFood::plan_intent(perception, danger_index, creature, entities);
    } else if (act == IntentActs::FIND_MATCH) {
        return PlannerFindMatch::plan_intent(perception, creature);
    } else if (act == IntentActs::NOTHING) {
        return PlannerNothing::plan_intent(perception, creature);
    }
    return std::nullopt;
}

std::optional<std::variant<FindFoodPresetsOutput, AttackPresetsOutput>> Planner::plan(
    const Perception& perception,
    const DangerIndex& danger_index,
    const Creature& creature,
    EntitiesRegistry& entities
) {
    std::optional<AttackOutput> result_attack = AttackResolver::resolve_attack(creature, perception, entities);
    
    if (!result_attack.has_value()) {
        return Planner::resolve_intent(perception, creature, danger_index, entities);
    }
    NormalizedValue score;
    if (creature.intent.act == IntentActs::FIND_FOOD) {
        score = ScorerIntents::score_find_food(creature);
    } else if (creature.intent.act == IntentActs::FIND_MATCH) {
        score = ScorerIntents::score_find_match(creature);
    } else {
        score = ScorerIntents::score_nothing(creature);
    }

    if (result_attack->score > score) {
        return PlannerAttack::plan_intent(result_attack.value(), perception, entities);
    } 
    return Planner::resolve_intent(perception, creature, danger_index, entities);
}