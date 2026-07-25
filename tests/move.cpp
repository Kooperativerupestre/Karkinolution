// tests/test_move.cpp

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/generators/catch_generators_random.hpp>

#include <karkinolution/organism/creatures.hpp>
#include <karkinolution/organism/ontology.hpp>
#include <karkinolution/world/world.hpp>
#include <karkinolution/terrain/map.hpp>
#include <karkinolution/brain/perception.hpp>
#include <karkinolution/actions/presets.hpp>
#include <karkinolution/actions/preset_executor.hpp>
#include <karkinolution/systems/physics.hpp>
#include <karkinolution/core/vec2.hpp>
#include <optional>
#include <utility>
#include <catch2/generators/catch_generators_adapters.hpp>

namespace {
struct Basic {
    World world;
    Perception perception;
    DangerIndex danger_index;
    Id creature_id;
};

Basic gen_basic() {
    World world = WorldFactory::create_world(PresetWorld{
        0,
        Vec2(10, 10),
        static_cast<float>(ScaleGenValues::MEDIUM)
    });

    Creature creature = CreatureFactory::gen_creature(CreatureConfig{.position = Vec2(0, 0)});
    Id creature_id = IDF::create_creature_id(creature.id);

    WorldMotor::add_entity(world, std::move(creature));

    Creature& stored = world.entities.at_creature(creature_id);
    Perception perception = Perceiver::perceive(stored, world.territory, world.entity_map, world.entities);
    DangerIndex danger_index = DangerFactory::create_danger_index(perception, stored);

    return Basic{std::move(world), std::move(perception), std::move(danger_index), creature_id};
}

} // namespace

TEST_CASE("Move preset execution") {
    Basic basic = gen_basic();
    Creature& creature = basic.world.entities.at_creature(basic.creature_id);

    SECTION("updates creature position on success") {
        Vec2 new_coord{0, 1};
        Vec2 old_position = creature.position;

        MoveOutputs output = PresetExecutor::execute_preset(
            MovePreset{new_coord}, creature, basic.perception, basic.danger_index, basic.world
        );

        if (output == MoveOutputs::OK) {
            REQUIRE(creature.position != old_position);
        } else {
            REQUIRE(creature.position == old_position);
        }
    }

    SECTION("vacates the old position only when the move succeeds") {
        int target_x = GENERATE(take(5, random(1, 2000)));
        int target_y = GENERATE(take(5, random(1, 2000)));

        Vec2 new_coord{target_x, target_y};
        Vec2 old_position = creature.position;

        MoveOutputs output = PresetExecutor::execute_preset(
            MovePreset{new_coord}, creature, basic.perception, basic.danger_index, basic.world
        );

        if (output == MoveOutputs::OK) {
            REQUIRE(creature.position != old_position);
        } else {
            REQUIRE(creature.position == old_position);
            REQUIRE(creature.position != new_coord);
        }
    }

    SECTION("keeps the entity map in sync with the move outcome") {
        Vec2 new_coord{0, 1};
        Vec2 old_position = creature.position;

        MoveOutputs output = PresetExecutor::execute_preset(
            MovePreset{new_coord}, creature, basic.perception, basic.danger_index, basic.world
        );

        if (output == MoveOutputs::OK) {
            REQUIRE(basic.world.entity_map.try_at(old_position) == nullptr);
        } else {
            const Id* stored_id = basic.world.entity_map.try_at(old_position);
            REQUIRE(stored_id != nullptr);
            REQUIRE(*stored_id == basic.creature_id);
        }
    }

    SECTION("consumes energy only when the move succeeds") {
        Vec2 new_coord{0, 1};
        float initial_energy = creature.energy.value();

        MoveOutputs output = PresetExecutor::execute_preset(
            MovePreset{new_coord}, creature, basic.perception, basic.danger_index, basic.world
        );

        if (output == MoveOutputs::OK) {
            REQUIRE(creature.energy.value() < initial_energy);
        } else {
            REQUIRE(creature.energy.value() == initial_energy);
        }
    }
}

TEST_CASE("Best position never lands on an already occupied coord") {
    Basic basic = gen_basic();
    Creature& creature_1 = basic.world.entities.at_creature(basic.creature_id);

    Creature creature_2 = CreatureFactory::gen_creature(CreatureConfig{.position = Vec2(0, 1)});
    Id creature_2_id = IDF::create_creature_id(creature_2.id);
    Vec2 creature_2_position = creature_2.position;

    WorldMotor::add_entity(basic.world, std::move(creature_2));
    Creature& stored_creature_2 = basic.world.entities.at_creature(creature_2_id);

    Perception perception_2 = Perceiver::perceive(
        stored_creature_2, basic.world.territory, basic.world.entity_map, basic.world.entities
    );
    DangerIndex danger_index_2 = DangerFactory::create_danger_index(perception_2, stored_creature_2);

    std::optional<Vec2> output = MovementSystem::find_best_pos(
        perception_2, danger_index_2, stored_creature_2, creature_1.position
    );

    if (output.has_value()) {
        REQUIRE(*output != creature_1.position);
        REQUIRE(
            Vec2F::distance(*output, creature_1.position)
            <= Vec2F::distance(creature_2_position, creature_1.position) + 1
        );
    }
}