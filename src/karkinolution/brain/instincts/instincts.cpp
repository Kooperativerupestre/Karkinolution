#include <karkinolution/brain/instincts/instincts.hpp>
#include "karkinolution/actions/actions.hpp"
#include "karkinolution/brain/perception.hpp"
#include "karkinolution/core/stats.hpp"
#include "karkinolution/core/vec2.hpp"
#include "karkinolution/organism/ontology.hpp"
#include "karkinolution/utils/k_random.hpp"
#include <optional>


// IntentScored

void IntentScored::operator*=(float value) {
    score *= value;
}

// AttackResolver
NormalizedValue AttackResolver::score_attack(const Creature& creature, const Creature& target) {
    return (creature.physical_ratio().value() - target.physical_ratio().value() + 1)/2;
}

std::optional<AttackOutput> AttackResolver::get_reactive_attack(
    const Creature& creature,
    const EntitiesRegistry& entities
) {
    if (creature.last_attack.has_value() && entities.exists(creature.last_attack.value().attacker_id)) {
        const Id target_id = creature.last_attack.value().attacker_id;
        NormalizedValue score = AttackResolver::score_attack(creature, entities.at_creature(target_id));

        return AttackOutput{
            .score = score,
            .target_id = target_id
        };
    }
    return std::nullopt;
}

std::optional<AttackOutput> AttackResolver::resolve_aggressive_attack(
    const Creature& creature,
    const Perception& perception,
    const EntitiesRegistry& entities
) {
    auto target_coord = AttackResolver::resolve_predicate_attack(
        creature,
        AGGRESSIVE_ATTACK_RADIUS,
        perception,
        [&creature](const PerceivedBlock&block, Vec2 coord)
                   { return block.get_creature().ontology.specie == creature.genome.core.specie; }
    );

    if (target_coord != std::nullopt) {
        auto target_id = perception.at(target_coord.value()).get_creature().id;
        auto score = AttackResolver::score_attack(creature, entities.at_creature(target_id));
        return AttackOutput(
            score,
            target_id
        );
    }
    return std::nullopt;
}

std::optional<AttackOutput> AttackResolver::resolve_territorial_attaqck(
    const Creature& creature,
    const Perception& perception,
    const EntitiesRegistry& entities
) {
    auto target_coord = AttackResolver::resolve_predicate_attack(
        creature,
        AGGRESSIVE_ATTACK_RADIUS,
        perception,
        [&creature](const PerceivedBlock&block, Vec2 coord)
        { return block.get_creature().ontology.specie != creature.genome.core.specie; }
    );
    if (target_coord == std::nullopt) {
        return std::nullopt;
    }

    auto target_id = perception.at(target_coord.value()).get_creature().id;
    NormalizedValue score = AttackResolver::score_attack(creature, entities.at_creature(target_id));
    if (creature.pregnant()) {
        score -= Factors::get_pregnant_territorial_avoidance_attack_factor(creature);
    }
    return AttackOutput{
        .score = AttackResolver::score_attack(creature, entities.at_creature(target_id)),
        .target_id = target_id
    };
}

std::optional<AttackOutput> AttackResolver::resolve_attack(
    const Creature& creature,
    const Perception& perception,
    const EntitiesRegistry& entities
) {
    const auto temperament = creature.genome.core.temperament;

    auto attack_output = AttackResolver::get_reactive_attack(creature, entities);

    if (attack_output == std::nullopt) {
        if (temperament == Temperament::AGGRESSIVE) {
            attack_output = AttackResolver::resolve_aggressive_attack(creature, perception, entities);
            if (attack_output != std::nullopt) {
                return attack_output;
            }
        } else if (temperament == Temperament::TERRITORIAL) {
            attack_output = AttackResolver::resolve_territorial_attaqck(creature, perception, entities);
            if (attack_output != std::nullopt) {
                return attack_output;
            }
        }
    }

    return std::nullopt;
}

// ==========================================
// ScorerIntents Namespace
// ==========================================

NormalizedValue ScorerIntents::score_find_food(const Creature& creature) {
    NormalizedValue factor;

    NormalizedValue hungry = creature.hungry();

    if (hungry.value() > creature.genome.metabolism.max_hungry) {
        factor += hungry * 1.3;
    } else {
        factor += hungry * 1.12;
    }

    if (creature.pregnant()) {
        factor += std::get<PregnantUterus>(creature.uterus).gravity();
    }
    return factor;
}

NormalizedValue ScorerIntents::score_find_match(const Creature& creature) {
    NormalizedValue factor;

    factor += creature.reproductive_maturity() * 0.9;

    factor *= creature.reproductive_fitness();

    return factor;
}

NormalizedValue ScorerIntents::score_nothing(const Creature& creature) {
    NormalizedValue base{0.6};
    base += creature.senescence()/4;
    return base;
}

// Instincts

IntentActs Instincts::take(
    const Creature& creature,
    ReproductiveBuffer& reproductive_buffer
) {
    std::vector<IntentScored> acts;

    // Nothing
    acts.push_back(IntentScored{
        IntentActs::NOTHING,
        ScorerIntents::score_nothing(creature) + Disturbs::gen_disturb(1.0f, 1.0f + NOISY_INSTINCTS)
    });
    
    // Find Food
    acts.push_back(IntentScored{
        IntentActs::FIND_FOOD,
        ScorerIntents::score_find_food(creature) + Disturbs::gen_disturb(1.0f, 1.0f + NOISY_INSTINCTS)
    });

    // Find Match
    if (creature.reproductively_capable() && reproductive_buffer.try_at(IDF::create_creature_id(creature.id)) != nullptr) {
        acts.push_back(IntentScored{
            IntentActs::FIND_MATCH,
            ScorerIntents::score_find_match(creature) * Disturbs::gen_disturb(1.0f, 1.0f + NOISY_INSTINCTS)
        });
    }

    auto chosen = acts[0];

    for (size_t i = 1; i < acts.size(); i++) {
        if (acts[i].score > chosen.score) {
            chosen = acts[i];
        }
    }
    return chosen.act;
}




// ==========================================
// Free Functions
// ==========================================

void try_call_reproductive_buffer(
    const Intent& intent,
    const Creature& creature,
    ReproductiveBuffer& reproductive_buffer
) {
    if (intent.act == IntentActs::FIND_MATCH) {
        reproductive_buffer.try_add(IDF::create_creature_id(creature.id), ReproductiveDesire(IDF::create_creature_id(creature.id), creature.genome.core.specie));
    }
}