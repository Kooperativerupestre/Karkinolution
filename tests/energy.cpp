// tests/test_metabolism.cpp

#include <catch2/catch_test_macros.hpp>

#include <karkinolution/organism/creatures.hpp>
#include <karkinolution/core/stats.hpp>
#include <karkinolution/systems/metabolism.hpp>
#include <karkinolution/core/global_epsilon.hpp>

TEST_CASE("Energy never goes negative") {
    Creature creature = CreatureFactory::gen_creature(CreatureConfig{});
    creature.energy -= (creature.energy.max());

    REQUIRE(creature.energy.value() >= Approx<float>(0.0f));
}

TEST_CASE("Feeding a creature restores energy") {
    Creature creature = CreatureFactory::gen_creature(CreatureConfig{});
    creature.energy.zero();
    Energy energy{1000.0f, 1000.0f};

    MetabolismSystem::eat(creature, energy, FoodHint::GRASS);

    REQUIRE(creature.energy.value() > Approx<float>(0.0f));
}

