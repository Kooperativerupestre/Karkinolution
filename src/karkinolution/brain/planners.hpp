#pragma once
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

using FindFoodPresetsOutput = std::variant<MovePreset, EatPreset, AttackPreset>;
using AttackPresetsOutput = std::variant<MovePreset, AttackPreset>;
using PlannAllPresetsOutput = std::variant<MovePreset, EatPreset, AttackPreset>;


inline constexpr float NOISY_NOTHING_INTENT = 0.619f;
inline constexpr float NOISY_HITS_TO_KILL = 0.1f;
inline constexpr float MIN_DIET_PREFERENCE_EFFECTIVENESS = 0.25f;

struct ScoredPreset {
    std::variant<EatPreset, MovePreset, AttackPreset, AttackPreset> preset;
    NormalizedValue score;
};

struct FoodCoord {
    Vec2 coord;
    FoodHint hint;
    SignedNormalizedValue score;
};

namespace PlannerFindFood {
    bool can_eat_food_option(const FoodOption&food_option, const Creature&creature);
    
    SignedNormalizedValue score_food_option(
        const FoodOption&food_option,
        const Creature&creature,
        const DangerIndex&danger_index,
        const Perception&perception
    );

    std::optional<FoodCoord> choose_best_food_coord(
        const Perception&perception,
        const DangerIndex&danger_index,
        const Creature&creature
    );

    FindFoodPresetsOutput decide_preset(const FoodCoord&food_coord, const Perception&perception, EntitiesRegistry&entities);
    std::optional<FindFoodPresetsOutput> plan_intent(
        const Perception&perception,
        const DangerIndex&danger_index,
        const Creature&creature,
        EntitiesRegistry&entities
    );
}

namespace PlannerFindMatch {
    std::optional<MovePreset> plan_intent(const Perception&perception, const Creature&creature);
}

namespace PlannerAttack {
    std::optional<AttackPresetsOutput> plan_intent(
        const AttackOutput&attack_input,
        const Perception&perception,
        const EntitiesRegistry&entities
    );
}

namespace PlannerNothing {
    NormalizedValue score_coord(const Perception&perception, Vec2 coord, const Creature&creature);
    std::optional<MovePreset> plan_intent(const Perception&perception, const Creature&creature);
}

namespace Planner {
    std::optional<std::variant<MovePreset, EatPreset, AttackPreset>> resolve_intent(
        const Perception&perception,
        const Creature&creature,
        const DangerIndex&danger_index,
        EntitiesRegistry&entities
    );
    std::optional<std::variant<FindFoodPresetsOutput, AttackPresetsOutput>> plan(
        const Perception&perception,
        const DangerIndex&danger_index,
        const Creature&creature,
        EntitiesRegistry&entities
    );
}