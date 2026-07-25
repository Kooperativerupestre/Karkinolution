#pragma once
#include <karkinolution/terrain/map.hpp>
#include <karkinolution/terrain/soil.hpp>
#include <karkinolution/actions/actions.hpp>
#include <karkinolution/core/stats.hpp>
#include <karkinolution/organism/genetics.hpp>
#include <karkinolution/organism/ontology.hpp>
#include <optional>
#include <karkinolution/core/vec2.hpp>
#include <ranges>
#include <karkinolution/organism/creatures.hpp>
#include <variant>




struct PerceivedBody {
    Energy energy;
    float life;
    NormalizedValue physical_ratio;
    bool reproductive_capacity;
};

struct PerceivedOntology {
    CreatureSpecies specie;
    Temperament temperament;
    Gender gender;
};

struct PerceivedCreature {
    PerceivedBody body;
    PerceivedOntology ontology;
    Id id;
};

struct PerceivedCorpse {
    Energy energy;
    Id id;
};

struct PerceivedCell {
    bool is_movable, is_edible, is_dangerous;

    std::optional<float> movement_cost, damage;
    std::optional<Energy> food;
    
    std::unordered_set<MoveActions> required_capabilities;
};

struct ObserverCreature {
    NormalizedValue energy_ratio, life_ratio;
    CreatureSpecies specie;
    Id id;
};

using PerceivedEntity = std::variant<std::monostate, PerceivedCreature, PerceivedCorpse>; 


struct PerceivedBlock {
    PerceivedCell cell;
    PerceivedEntity entity;
    float distance;

    bool has_entity() const;
    std::optional<EntityTypes> get_entity_type() const;
    bool has_creature() const;
    bool has_corpse() const;

    const PerceivedCreature& get_creature() const;
    const PerceivedCorpse& get_corpse() const;
};


namespace BlockProperties {
    bool has_aggressive_creature(const PerceivedBlock& block);
    bool has_territorial_creature(const PerceivedBlock& block);
    bool is_same_specie(const PerceivedCreature& other, const ObserverCreature& creature);
    bool is_more_strong(const PerceivedCreature& other, const ObserverCreature& creature);
};

using const_iterator = std::unordered_map<Vec2, PerceivedBlock>::const_iterator;

using DefaultPredicateType = decltype([](const PerceivedBlock&block, Vec2 coord) { return true; });

inline constexpr DefaultPredicateType default_predicate{};

class Perception {
    private:

    std::unordered_map<Vec2, PerceivedBlock> pieces;
    ObserverCreature _creature;
    Vec2 _coord;
    float _max_distance;

    public:

    Perception(const std::unordered_map<Vec2, PerceivedBlock>& pieces,
                const ObserverCreature& creature,
                Vec2 coord,
                float max_distance
    ): pieces(pieces), _creature(creature), _coord(coord), _max_distance(max_distance) {}

    const_iterator begin() const;

    const_iterator end() const;
    const std::unordered_map<Vec2, PerceivedBlock>& pieces_ref() const;
    int size() const;
    bool contains(Vec2 coord) const;
    bool empty() const;
    const PerceivedBlock& at(Vec2 coord) const;
    const PerceivedBlock* try_at(Vec2 coord) const;

    auto coords_view() const {
        return pieces | std::views::keys;
    }
    
    const PerceivedBlock& creature_block() const;

    const ObserverCreature& creature() const;
    Vec2 coord() const;
    float max_distance() const;
};

namespace PerceptionAnalyser {
    Perception build_another_perception(const Perception& old_perception,
         const std::unordered_map<Vec2, PerceivedBlock>& new_pieces);

    template <typename Predicate = DefaultPredicateType>
    Perception find_predicate(
        const Perception& perception,
        Predicate predicate = default_predicate
    ) {
        std::unordered_map<Vec2, PerceivedBlock> new_pieces;

        for (const auto& [c, b] : perception.pieces_ref()) {
            if (predicate(b, c)) {
                new_pieces[c] = b;
            }
        }

        return PerceptionAnalyser::build_another_perception(perception, new_pieces);
    }

    template <typename Predicate = DefaultPredicateType, std::ranges::range R>
    Perception build(
        const Perception& perception,
        const R& coords,
        Predicate predicate = default_predicate
    ) {
        std::unordered_map<Vec2, PerceivedBlock> new_pieces;

        for (auto coord : coords) {
            const PerceivedBlock* block = perception.try_at(coord);

            if (block != nullptr && predicate(*block, coord)) {
                new_pieces[coord] = *block;
            }
        }

        return PerceptionAnalyser::build_another_perception(perception, new_pieces);
    }
    
    Perception neighbors_4(const Perception& perception, bool include_self = false);
    Perception neighbors_8(const Perception& perception, bool include_self = false);

    template <typename Predicate = DefaultPredicateType>
    Perception neighbors_x_y(
        const Perception& perception,
        Vec2 radius,
        Predicate predicate = default_predicate
    ) {
        std::unordered_map<Vec2, PerceivedBlock> filtered;
        Vec2 center = perception.coord();

        for (int x = -radius.x; x <= radius.x; ++x) {
            for (int y = -radius.y; y <= radius.y; ++y) {
                Vec2 target = center + Vec2{x, y};

                const PerceivedBlock* block = perception.try_at(target);
                if (block != nullptr && predicate(*block, Vec2{x, y})) {
                    filtered[target] = *block;
                }
            }
        }

        return PerceptionAnalyser::build_another_perception(perception, filtered);
    }
        

    Vec2 near_coord(const Perception&perception);

    Perception get_area_in_radius_ratio(const Perception& perception, float radius_ratio);
};

namespace PerceptionPatterns {

    template <typename Predicate>
    Perception empty_spaces(const Perception perception, Predicate predicate) {
        return PerceptionAnalyser::find_predicate(
            perception,
            [](const PerceivedBlock& block, Vec2 coord) {
                return !block.has_entity();
            }
        );
    }

    template <typename Predicate>
    Perception find_corpses(const Perception& perception, Predicate predicate) {
        return PerceptionAnalyser::find_predicate(
            perception,
            [](const PerceivedBlock& block, Vec2 coord) {
                return block.has_corpse();
            }
        );
    }

    template <typename Predicate>
    Perception find_creatures(const Perception& perception, Predicate predicate) {
        return PerceptionAnalyser::find_predicate(
            perception,
            [](const PerceivedBlock& block, Vec2 coord) {
                return block.has_creature();
            }
        );
    }

    template <typename Predicate>
    Perception find_same_species(const Perception& perception, Predicate predicate) {
        return PerceptionAnalyser::find_predicate(
            perception,
            [&perception](const PerceivedBlock& block, Vec2 coord) {
                return block.has_creature() &&
                       BlockProperties::is_same_specie(
                           block.get_creature(),
                           perception.creature()
                       );
            }
        );
    }

    template <typename Predicate>
    Perception find_other_species(const Perception& perception, Predicate predicate) {
        return PerceptionAnalyser::find_predicate(
            perception,
            [&perception](const PerceivedBlock& block, Vec2 coord) {
                return block.has_creature() &&
                       !BlockProperties::is_same_specie(
                           block.get_creature(),
                           perception.creature()
                       );
            }
        );
    }

}

namespace Perceiver {
    PerceivedCreature perceive_creature(const Creature& creature);
    PerceivedCorpse perceive_corpse(const Corpse& corpse);
    PerceivedCell perceive_soil(const SoilPiece& soil_piece);

    Perception perceive(const Creature& creature, const Territory& territory,
    const EntityMap& entity_map, const EntitiesRegistry& entities);
}

struct Dangers {
    SignedNormalizedValue cell_danger, creature_danger;
    
    float accumulated_danger() const;
};

inline constexpr float TERRITORIAL_DANGER = 0.25f;
inline constexpr float AGGRESSIVE_DANGER = 0.3f;
inline constexpr float DANGEROUS_CELL_FACTOR = 0.18f;
inline constexpr float ADJACENT_DANGER_WEIGHT = 0.4f;
inline constexpr float GET_DANGER_INDEX_AREA_RATIO = 0.75f;

class DangerIndex {
    private:

    std::unordered_map<Vec2, Dangers> index;

    public:

    const Dangers& at(Vec2 coord) const;
    const Dangers* try_at(Vec2 coord) const;
    void add(Vec2 coord, const Dangers&);
};


namespace DangerFactory {
    SignedNormalizedValue get_creature_danger(const PerceivedBlock& block, const Creature& creature);
    SignedNormalizedValue get_cell_danger(const PerceivedBlock& block);
    Dangers get_dangers(const PerceivedBlock& block, const Creature& creature);
    DangerIndex create_danger_index(const Perception& perception, const Creature& creature);
};