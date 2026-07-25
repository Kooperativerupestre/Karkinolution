#include <karkinolution/brain/planners.hpp>
#include <karkinolution/core/vec2.hpp>
#include <karkinolution/core/stats.hpp>
#include <karkinolution/organism/ontology.hpp>
#include <karkinolution/systems/death.hpp>
#include <karkinolution/ticks/lifecycle.hpp>
#include <karkinolution/organism/creatures.hpp>
#include <karkinolution/world/world.hpp>
#include <karkinolution/resolvers/resolvers.hpp>
#include <karkinolution/brain/perception.hpp>
#include <optional>
#include <karkinolution/actions/presets.hpp>
#include <variant>
#include <vector>
#include <format>
#include <karkinolution/core/global_epsilon.hpp>
#include <karkinolution/utils/k_random.hpp>
#include <karkinolution/organism/validator.hpp>
#include <karkinolution/brain/intent.hpp>
#include <karkinolution/actions/preset_executor.hpp>

// --- TickResolvers ---

void TickResolvers::resolve_death(Creature& creature, World& world) {
    Corpse corpse = DeathSystem::generate_corpse(creature);
    world.add_log(std::format("Creature {} died", creature.name));
    WorldMotor::delete_entity(world, IDF::create_creature_id(creature.id));
    if (corpse.time_left() == Approx<float>(0)) {
        return;
    } WorldMotor::add_entity(world, std::move(corpse));
}

std::optional<Creature> TickResolvers::resolve_born(Creature& creature, const Perception& perception, World& world) {
    Validator::validate_pregnant(creature);
    auto possibilities = MovementSystem::four_movable_coords(perception, creature);
    

    if (possibilities.size() == 0) {
        NormalizedValue gravity = std::get<PregnantUterus>(creature.uterus).gravity();
        bool r = Choices::choice_bool(gravity.value(), 1 - gravity.value());
        
        if (r) {
            std::optional<BornData> born_data= ReproductiveSystem::to_birth(creature);
            Perception new_possibilities = PerceptionAnalyser::neighbors_8(perception);

            if (new_possibilities.size() == 0) {
                return std::nullopt;
            }

            Vec2 new_coord = Choices::choice(perception.coords_view());
            return BornResolver::resolve_born_data(born_data, new_coord, world, creature.name);
        }
        return std::nullopt;
    }
    std::optional<BornData> born_data = ReproductiveSystem::to_birth(creature);
    Vec2 new_coord = Choices::choice(possibilities);
    return BornResolver::resolve_born_data(born_data, new_coord, world, creature.name);
}

// --- RunnerCreature ---

void RunnerCreature::run_basic_fisiology(Creature& creature) {
    creature.energy -= creature.basal_metabolism();
    creature.age.pass();
    if (creature.pregnant()) {
        creature.energy -= std::get<PregnantUterus>(creature.uterus).pregnancy_cost();
    }
}

std::optional<Creature> RunnerCreature::run_uterus(Creature& creature, const Perception& perception, World& world) {
    if (creature.pregnant()) {
        UterusSystem::pass_time(creature);

        if (std::get<PregnantUterus>(creature.uterus).gestation.is_ready_to_born()) {
            return TickResolvers::resolve_born(creature, perception, world);
        }
    }
    return std::nullopt;
}

std::optional<ReproducePreset> RunnerCreature::try_to_get_reproduce_preset(Creature& creature, const Perception& perception, World& world) {
    if (world.reproductive_buffer.exists(IDF::create_creature_id(creature.id))) {
        return ReproductionResolver::resolve_reproduction(creature, perception, world.entities);
    }
    return std::nullopt;
}

std::vector<AllPresets> RunnerCreature::get_presets(Creature& creature, const Perception& perception,
                                                    const DangerIndex& danger_index, World& world) {
    std::vector<AllPresets> presets;
    std::optional<AllPresets> preset = IntentResolver::resolve_intent(creature, world, perception, danger_index);
    if (preset == std::nullopt) {
        preset = PlannerNothing::plan_intent(perception, creature);
    }
    
    if (preset != std::nullopt) {
        presets.push_back(preset.value());
    }
    preset = RunnerCreature::try_to_get_reproduce_preset(creature, perception, world);
    if (preset != std::nullopt) {
        presets.push_back(preset.value());
    }
    
    return presets;
}

void RunnerCreature::run_creature(Creature& creature, World& world) {
    std::optional<Creature> new_child = std::nullopt;
    bool is_dead = DeathSystem::is_dead(creature);

    if (is_dead) {
        TickResolvers::resolve_death(creature, world);
        return;
    }

    RunnerCreature::run_basic_fisiology(creature);

    Perception perception = Perceiver::perceive(creature, world.territory, world.entity_map, world.entities);
    DangerIndex danger_index = DangerFactory::create_danger_index(perception, creature);
    
    std::vector<AllPresets> presets = RunnerCreature::get_presets(creature, perception, danger_index, world);
    new_child = RunnerCreature::run_uterus(creature, perception, world);

    if (new_child.has_value()) {
        WorldMotor::add_entity(world, std::move(new_child.value()));
        for (const auto& preset : presets) {
            if (!std::holds_alternative<MovePreset>(preset)) {
                PresetExecutor::execute_generic_preset(preset, creature, perception, danger_index, world);
            }
        }
    } else {
        for (const auto& preset : presets) {
            PresetExecutor::execute_generic_preset(preset, creature, perception, danger_index, world);
        }
    }
}

// --- RunnerCorpse ---

void RunnerCorpse::to_degrade_corpse(Corpse& corpse) {
    corpse.energy *= (0.95 - corpse.decomposition_time.value()/100.0f);
    corpse.decomposition_time.pass();
}

void RunnerCorpse::run_corpse(Corpse& corpse, World& world) {
    if (corpse.ready_to_disapear()) {
        WorldMotor::delete_entity(world, IDF::create_corpse_id(corpse.id));
        return;
    } RunnerCorpse::to_degrade_corpse(corpse);
}