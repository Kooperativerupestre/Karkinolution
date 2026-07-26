// tests/test_world_presets_run.cpp

#include <catch2/catch_test_macros.hpp>

#include <karkinolution/world/world.hpp>
#include <karkinolution/ticks/worldcycle.hpp>
#include <format>

namespace {

void run_ticks(World& world, int ticks) {
    for (int tick = 0; tick < ticks; ++tick) {
        RunnerWorld::run(world);
    }
}

} // namespace

constexpr int TICKS = 20;

TEST_CASE(std::format("Crab chaos world survives {} ticks without crashing", TICKS)) {
    World world = WorldFactory::create_crab_chaos();

    REQUIRE_NOTHROW(run_ticks(world, TICKS));
}
TEST_CASE(std::format("Paranoic world survives {} ticks without crashing", TICKS)) {
    World world = WorldFactory::create_paranoic();

    REQUIRE_NOTHROW(run_ticks(world, TICKS));
}

TEST_CASE(std::format("Titanic world survives {} ticks without crashing", TICKS)) {
    World world = WorldFactory::create_titanic();

    REQUIRE_NOTHROW(run_ticks(world, TICKS));
}

TEST_CASE(std::format("Normal world survives {} ticks without crashing", TICKS)) {
    World world = WorldFactory::create_normal();

    REQUIRE_NOTHROW(run_ticks(world, TICKS));
}