#include <karkinolution/organism/genetics.hpp>
#include <karkinolution/brain/perception.hpp>
#include <karkinolution/systems/metabolism.hpp>
#include <karkinolution/organism/ontology.hpp>
#include <karkinolution/utils/k_random.hpp>
#include <karkinolution/core/stats.hpp>
#include <algorithm>

namespace MetabolismSystem {

    float get_food_yield(float energy, const FoodHint& food_hint)
    {
        if (food_hint == FoodHint::CORPSE) {
            return energy * CORPSE_FOOD_YIELD;
        } return energy;
    }


    std::vector<FoodOption> get_food_options(
        const Perception& perception,
        Vec2 coord
    )
    {
        std::vector<FoodOption> food_options{};
        const PerceivedBlock* block = perception.try_at(coord);

        if (block->cell.is_edible) {
            food_options.push_back(
                FoodOption{
                    .coord = coord,
                    .hint = FoodHint::GRASS,
                    .energy_gain = block->cell.food.value().value()
                }
            );
        }

        if (block->has_corpse()) {
            food_options.push_back(
                FoodOption{
                    .coord = coord,
                    .hint = FoodHint::CORPSE,
                    .energy_gain = std::get<PerceivedCorpse>(block->entity).energy.value()
                }
            );
        }

        if (block->has_creature()) {
            food_options.push_back(
                FoodOption{
                    .coord = coord,
                    .hint = FoodHint::TARGET,
                    .energy_gain = std::get<PerceivedCreature>(block->entity).body.energy.value()
                }
            );
        }
        return food_options;
    }


    float diet_effective(float diet_bias, NormalizedValue hungry)
    {
        return 1.0f + (1.0f - hungry.value()) * (diet_bias - 1.0f);
    }


    void eat(
        Creature& creature,
        Energy& energy,
        const FoodHint& food_hint
    )
    {
        float needed = creature.needed_energy();

        if (creature.pregnant()) {
            needed *= Disturbs::gen_disturb(0.95f, 1.05f);
        } else {
            needed *= Disturbs::gen_disturb(0.90f, 1.10f);
        }

        float extracted = MetabolismSystem::get_food_yield(energy.value(), food_hint);
        float amount = std::min(needed, extracted);

        energy -= amount;
        creature.energy += amount;
    }
}
