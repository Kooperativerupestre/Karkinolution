#include "karkinolution/brain/perception.hpp"
#include "karkinolution/core/error.hpp"
#include "karkinolution/core/stats.hpp"
#include "karkinolution/core/vec2.hpp"
#include "karkinolution/organism/ontology.hpp"
#include "karkinolution/terrain/soil.hpp"
#include <karkinolution/systems/physics.hpp>
#include <cmath>
#include <limits>
#include <format>

namespace SpatialSystem {
    bool can_move(const PerceivedBlock& block, const std::unordered_set<MoveActions>& capabilities) {
        return is_subset(block.cell.required_capabilities, capabilities) && ! block.has_entity();
    }

    bool can_go(const PerceivedBlock& block, const std::unordered_set<MoveActions>& capabilities) {
        return is_subset(block.cell.required_capabilities, capabilities);
    }

    std::vector<Effect> get_effects(const Perception& perception, const Territory& territory, const Creature& creature) {
        std::vector<Effect> effects;
        
        const SoilPiece* soil = &territory.at(perception.coord());
        const PerceivedBlock* block = &perception.creature_block();

        if (auto *damage_component = soil->components.try_get<Damage>()) {
            float damage = damage_component->damage;
            effects.push_back([damage](Creature&creature) { creature.life -= damage; } );
        }

        if (!SpatialSystem::can_go(*block, creature.genome.core.capabilities)) {
            effects.push_back([](Creature&creature) { creature.life *= 0.10; });
        }
        return effects;
    }

    void apply_effects(const std::vector<Effect>& effects, Creature& creature) {
        for (auto effect : effects) {
            if (effect) {
                effect(creature);
            }
        }
    }
}

namespace MovementSystem {
    float calculate_cost_to_move(Vec2 new_coord, const Creature& creature, const Territory& territory) {
        const SoilPiece* soil = &territory.at(new_coord);
        
        if (!soil->components.exists<MovementCost>()) {
            throw NonMotileError{std::format("Soil of coord ({}, {})", new_coord.x, new_coord.y)};
        }
        float cost = soil->components.try_get<MovementCost>()->cost;
        return cost * creature.genome.metabolism.mass;
    }

    float calculate_cost_distance_to_move(float distance, const Creature& creature) {
        return creature.genome.metabolism.mass * distance;
    }

    NormalizedValue score_pos(const Perception& perception, const DangerIndex& danger_index, Vec2 init_coord, Vec2 end_coord) {
        NormalizedValue score{Vec2F::distance(init_coord, end_coord)/perception.max_distance()};

        const Dangers* danger = danger_index.try_at(end_coord);
        if (danger == nullptr) {
            score -= 0.17;
        } else {
            score -= danger->accumulated_danger();
        }
        score -= Vec2F::distance(perception.coord(), end_coord);

        return score;
    }

    std::vector<Vec2> four_movable_coords(const Perception& perception, const Creature& creature) {
        auto four_neighbors = PerceptionAnalyser::neighbors_4(perception);

        std::vector<Vec2> valids{};

        for (const auto& [c, b] : four_neighbors) {
            if (SpatialSystem::can_move(b, creature.genome.core.capabilities)) {
                valids.push_back(c);
            }
        }
        return valids;
    }

    std::optional<Vec2> find_best_pos(const Perception& perception, const DangerIndex& danger_index, const Creature& creature, Vec2 coord) {
        std::vector<Vec2> four_coords = MovementSystem::four_movable_coords(perception, creature);

        if (four_coords.size() == 0) {
            return std::nullopt;
        }

        std::vector<ScoredCoord> options{};

        ScoredCoord best = {four_coords[0], MovementSystem::score_pos(perception, danger_index, four_coords[0], coord)};
        NormalizedValue new_score;
        for (auto f_c : four_coords) {
            new_score = MovementSystem::score_pos(perception, danger_index, f_c, coord);
            if (new_score > best.score) {
                best.score = new_score;
                best.coord = f_c;
            }
        }
        return best.coord;
    }
}

namespace AttackSystem {
    float calculate_cost_to_attack(const Creature& creature) {
        return 1.7f * creature.genome.metabolism.mass;
    }

    int calculate_attacks_to_kill(float creature_strength, float target_life) {
        if (creature_strength <= 0.0f) {
            return std::numeric_limits<int>::max();
        }
        return static_cast<int>(std::ceil(target_life / creature_strength));
    }

    float calculate_cost_to_kill(const Creature& creature, const PerceivedCreature& target) {
        float cost_atk = AttackSystem::calculate_cost_to_attack(creature);
        float atks = static_cast<float>(AttackSystem::calculate_attacks_to_kill(creature.genome.body.strength, target.body.life));
        return cost_atk * atks;
    }

    float attack(const Creature& creature, Creature& target) {
        AttackedEvent attack_event{.attacker_id = IDF::create_creature_id(creature.id), .damage=creature.genome.body.strength};

        float cost = AttackSystem::calculate_cost_to_attack(creature);
        target.life -= creature.genome.body.strength;

        target.last_attack = attack_event;
        return cost;
    }

    bool distance_can_attack(Vec2 coord_creature, Vec2 coord_target) {
        return not Vec2F::distance_exceeds_one(coord_creature, coord_target);
    }
};