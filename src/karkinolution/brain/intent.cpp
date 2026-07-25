#include <karkinolution/brain/intent.hpp>
#include <karkinolution/organism/creatures.hpp>
#include <karkinolution/actions/actions.hpp>
#include <karkinolution/world/world.hpp>
#include <karkinolution/systems/reproductivebuffer.hpp>
#include <karkinolution/actions/presets.hpp>
#include <optional>
#include <karkinolution/brain/perception.hpp>
#include <karkinolution/brain/instincts/instincts.hpp>
#include <karkinolution/brain/planners.hpp>

namespace IntentResolver {

    void to_nothing_intent(Creature& creature) {
        creature.intent = Intent(IntentActs::NOTHING);
    }

    void cancel_invalid_intents(Creature& creature) {
        const IntentActs act = creature.intent.act;
        const int time = creature.intent.time;

        if (creature.hungry().value() < creature.genome.metabolism.max_hungry && time > 2 && act == IntentActs::FIND_FOOD) {
            IntentResolver::to_nothing_intent(creature);
        }

        if (time > 5 && act == IntentActs::FIND_MATCH) {
            IntentResolver::to_nothing_intent(creature);
        }
    }

    void update_intent(Creature& creature, ReproductiveBuffer& reproductive_buffer) {
        creature.intent.time += 1;
        if (creature.intent.act == IntentActs::NOTHING) {
            IntentActs act = Instincts::take(creature, reproductive_buffer);

            creature.intent = Intent(act);
            try_call_reproductive_buffer(creature.intent, creature, reproductive_buffer);
        }
    }

    std::optional<AllPresets> resolve_intent(
        Creature& creature, 
        World& world, 
        const Perception& perception, 
        const DangerIndex& danger_index
    ) {
        IntentResolver::cancel_invalid_intents(creature);
        IntentResolver::update_intent(creature, world.reproductive_buffer);

        std::optional<std::variant<FindFoodPresetsOutput, AttackPresetsOutput>> result =  Planner::plan(perception, danger_index, creature, world.entities);

        if (!result)
            return std::nullopt;

        return std::visit(
            [](const auto& variant) -> AllPresets {
                return std::visit(
                    [](const auto& preset) -> AllPresets {
                        return preset;
                    },
                    variant
                );
            },
            *result
        );
    }
} // namespace IntentResolver