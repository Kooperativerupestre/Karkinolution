#include <catch2/catch_test_macros.hpp>

#include <karkinolution/terrain/map.hpp>
#include <karkinolution/world/world.hpp>

TEST_CASE("random unoccupied coords returns unique positions") {
    World world = WorldFactory::create_world(PresetWorld{
        0,
        Vec2(3, 3),
        static_cast<float>(ScaleGenValues::MEDIUM)
    });

    auto coords = TerrainQuery::random_unoccupied_coords(world.entity_map, world.territory, 5);

    bool has_duplicates = false;
    for (size_t i = 0; i < coords.size(); ++i) {
        for (size_t j = i + 1; j < coords.size(); ++j) {
            if (coords[i] == coords[j]) {
                has_duplicates = true;
                break;
            }
        }
    }

    REQUIRE_FALSE(has_duplicates);
}
