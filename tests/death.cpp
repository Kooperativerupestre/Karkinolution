// tests/test_death.cpp

#include <catch2/catch_test_macros.hpp>

#include <karkinolution/organism/creatures.hpp>
#include <karkinolution/systems/death.hpp>

TEST_CASE("Creature dies under different conditions") {
    Creature creature = CreatureFactory::gen_creature(CreatureConfig{});

    SECTION("dies when energy reaches zero") {
        creature.energy.zero();
        REQUIRE(DeathSystem::is_dead(creature));
    }

    SECTION("dies when life reaches zero") {
        creature.life.zero();
        REQUIRE(DeathSystem::is_dead(creature));
    }

    SECTION("dies when age reaches maximum") {
        creature.age.full();
        REQUIRE(DeathSystem::is_dead(creature));
    }
}