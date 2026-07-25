#pragma once
#include <karkinolution/core/stats.hpp>
#include <karkinolution/core/vec2.hpp>
#include <karkinolution/core/error.hpp>
#include <karkinolution/actions/actions.hpp>
#include <karkinolution/brain/perception.hpp>
#include <karkinolution/organism/creatures.hpp>
#include <karkinolution/terrain/map.hpp>
#include <karkinolution/terrain/soil.hpp>
#include <functional>
#include <karkinolution/utils/dataclasses.hpp>

using Effect = std::function<void(Creature&)>;

template <typename T>
bool is_subset(const std::unordered_set<T>& sub, const std::unordered_set<T>& super) {
    if (sub.size() > super.size()) return false;

    for (const auto& elem : sub) {
        if (super.find(elem) == super.end()) {
            return false;
        }
    }
    return true;
}

namespace SpatialSystem {
    bool can_move(const PerceivedBlock&block, const std::unordered_set<MoveActions>& capabilities);
    bool can_go(const PerceivedBlock&block, const std::unordered_set<MoveActions>& capabilities);
    std::vector<Effect> get_effects(const Perception&perception, const Territory&territory, const Creature&creature);
    void apply_effects(const std::vector<Effect>& effects, Creature&);
}

namespace MovementSystem {
    float calculate_cost_to_move(Vec2 new_coord, const Creature&creature, const Territory&territory);
    float calculate_cost_distance_to_move(float distance, const Creature&creature);
    NormalizedValue score_pos(const Perception&perception, const DangerIndex&danger_index, Vec2 init_coord, Vec2 end_coord);
    std::vector<Vec2> four_movable_coords(const Perception&perception, const Creature&creature);
    std::optional<Vec2> find_best_pos(const Perception&perception, const DangerIndex&danger_index, const Creature&creature, Vec2 coord);
}

namespace AttackSystem {
    float calculate_cost_to_attack(const Creature&creature);
    int calculate_attacks_to_kill(float creature_strength, float target_life);
    float calculate_cost_to_kill(const Creature&creature, const PerceivedCreature&target);
    float attack(const Creature&creature, Creature&target);
    bool distance_can_attack(Vec2 coord_creature, Vec2 coord_target);
}
