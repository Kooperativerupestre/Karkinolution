#pragma once
#include <karkinolution/organism/ontology.hpp>
#include <karkinolution/organism/creatures.hpp>
#include <karkinolution/core/stats.hpp>
#include <karkinolution/brain/instincts/factors.hpp>
#include <optional>
#include <karkinolution/core/vec2.hpp>
#include <karkinolution/brain/perception.hpp>
#include <karkinolution/actions/actions.hpp>
#include <variant>
#include <karkinolution/systems/reproductivebuffer.hpp>

inline constexpr float NOISY_INSTINCTS = 0.1f;
inline constexpr Vec2 AGGRESSIVE_ATTACK_RADIUS = Vec2{3, 2};
inline constexpr Vec2 REACTIVE_ATTACK_RADIUS = Vec2{2, 2};


struct AttackOutput {
    NormalizedValue score;
    Id target_id;
};

namespace AttackResolver {
    NormalizedValue score_attack(const Creature&creature, const Creature&target);
    std::optional<AttackOutput> get_reactive_attack(const Creature&creature, const EntitiesRegistry&entities);
    
    template <typename Predicate>
    std::optional<Vec2> resolve_predicate_attack(
        const Creature&creature,
        Vec2 radius,
        const Perception&perception,
        Predicate predicate = [](const PerceivedBlock&block, Vec2 coord){ return true; }
    ) {
        auto area = PerceptionAnalyser::neighbors_x_y(perception, radius,
            [predicate](const PerceivedBlock&block, Vec2 coord){ return std::holds_alternative<PerceivedCreature>(block.entity) && predicate(block, coord); });
        
        if (area.empty()) {
            return std::nullopt;
        }

        return PerceptionAnalyser::near_coord(area);   // usa o filtrado
    }

    std::optional<AttackOutput> resolve_aggressive_attack(
        const Creature&creature,
        const Perception&perception,
        const EntitiesRegistry&entities
    );

    std::optional<AttackOutput> resolve_territorial_attaqck(
        const Creature&creature,
        const Perception&perception,
        const EntitiesRegistry&entities
    );

    std::optional<AttackOutput> resolve_attack(
        const Creature&creature,
        const Perception&perception,
        const EntitiesRegistry&entities
    );
}

namespace ScorerIntents {
    NormalizedValue score_find_food(const Creature&creature);
    NormalizedValue score_find_match(const Creature&creature);
    NormalizedValue score_nothing(const Creature&creature);
}

struct IntentScored {
    IntentActs act;
    NormalizedValue score;

    void operator*=(float value);
};

namespace Instincts {
    IntentActs take(const Creature&creature, ReproductiveBuffer&reproductive_buffer);
}

void try_call_reproductive_buffer(const Intent&intent, const Creature&creature, ReproductiveBuffer&reproductive_buffer);

