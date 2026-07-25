#include <karkinolution/organism/ontology.hpp>
#include <karkinolution/systems/reproduction.hpp>
#include <karkinolution/brain/perception.hpp>
#include <karkinolution/organism/creatures.hpp>
#include <karkinolution/core/vec2.hpp>
#include <karkinolution/systems/physics.hpp>
#include <karkinolution/systems/metabolism.hpp>
#include <karkinolution/actions/presets.hpp>
#include <karkinolution/world/world.hpp>
#include <karkinolution/actions/preset_executor.hpp>
#include <format>
#include <variant>

void PresetExecutor::execute_preset(const ReproducePreset& preset, World& world) {
    auto& buffer = world.reproductive_buffer;
    auto& entities = world.entities;

    Creature& female = entities.at_creature(preset.female);
    Creature& male = entities.at_creature(preset.male);

    auto costs = ReproductiveSystem::reproduce(female, male);
    if (!costs.has_value()) {
        return;
    }
    female.energy -= (costs.value().female_cost);
    male.energy -= (costs.value().male_cost);

    buffer.try_del(IDF::create_creature_id(female.id));
    buffer.try_del(IDF::create_creature_id(male.id));

    world.add_log(std::format("Creatures {} & {} reproduced", female.name, male.name));
}

void PresetExecutor::execute_preset(const EatPreset& preset, Creature& creature, World& world) {
    MetabolismSystem::eat(creature, preset.energy, preset.food_hint);
    world.add_log(std::format("Creature {} ate", creature.name));
}

MoveOutputs PresetExecutor::execute_preset(const MovePreset& preset, Creature& creature,
                                           const Perception& perception, const DangerIndex& danger_index,
                                           World& world) {
    std::optional<Vec2> best_pos = MovementSystem::find_best_pos(perception, danger_index, creature, preset.new_coord);

    if (!best_pos.has_value()) {
        return MoveOutputs::CANNOT_GET_BEST_POSITION;
    }

    float cost = MovementSystem::calculate_cost_to_move(best_pos.value(), creature, world.territory);
    if (creature.energy.value() < cost) {
        return MoveOutputs::INSUFFICIENT_ENERGY;
    }
    TerrainMotor::move(creature.position, best_pos.value(), world.entity_map, world.territory);
    creature.energy -= cost;
    creature.position = best_pos.value();
    world.add_log(std::format("Creature {} moved to ({}, {})", creature.name, best_pos.value().x, best_pos.value().y));
    return MoveOutputs::OK;
}

void PresetExecutor::execute_preset(const AttackPreset& preset, Creature& creature, World& world) {
    Creature& target = world.entities.at_creature(preset.target);

    float cost = AttackSystem::attack(creature, target);
}


void PresetExecutor::execute_generic_preset(const AllPresets &preset, Creature &creature, const Perception&perception, const DangerIndex&danger_index, World &world) {
    if (std::holds_alternative<AttackPreset>(preset)) {
        PresetExecutor::execute_preset(std::get<AttackPreset>(preset), creature, world);
    } else if (std::holds_alternative<EatPreset>(preset)) {
        PresetExecutor::execute_preset(std::get<EatPreset>(preset), creature, world);
    } else if (std::holds_alternative<ReproducePreset>(preset)) {
        PresetExecutor::execute_preset(std::get<ReproducePreset>(preset), world);
    } else if (std::holds_alternative<MovePreset>(preset)) {
        PresetExecutor::execute_preset(std::get<MovePreset>(preset), creature, perception, danger_index, world);
    }
}