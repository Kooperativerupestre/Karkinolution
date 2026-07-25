#include <karkinolution/ticks/worldcycle.hpp>
#include <karkinolution/organism/ontology.hpp>
#include <karkinolution/world/world.hpp>
#include <karkinolution/ticks/lifecycle.hpp>
#include <karkinolution/terrain/soil.hpp>

void RunnerWorld::run(World& world) {
    for (auto& soil : world.territory.values()) {
        if (soil.components.exists<FoodState>()) {
            auto* ptr = soil.components.try_get<FoodState>();
            ptr->food += ptr->regen_tax;
        }
    }

    std::vector<Id> entities_to_run;

    for (const auto& [coord, id] : world.entity_map.iter()) {
        entities_to_run.push_back(id);
    }

    for (const auto& id : entities_to_run) {
        if (id.entity_type == EntityTypes::CREATURE) {
            RunnerCreature::run_creature(
                world.entities.at_creature(id),
                world
            );
        } 
        else if (id.entity_type == EntityTypes::CORPSE) {
            RunnerCorpse::run_corpse(
                world.entities.at_corpse(id),
                world
            );
        }
    }

    world.time += 1;
}