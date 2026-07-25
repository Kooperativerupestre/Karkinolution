#include "karkinolution/organism/ontology.hpp"
#include <karkinolution/world/world.hpp>
#include <karkinolution/organism/creatures.hpp>
#include <karkinolution/terrain/map.hpp>
#include <karkinolution/core/vec2.hpp>

namespace WorldMotor {

    void add_entity(World& world, Entity&& entity) {
        Id id = EntityGetters::get_id(entity);
        TerrainMotor::add_entity(id, EntityGetters::get_position(entity), world.territory, world.entity_map);
        world.entities.add(id, std::move(entity));
    }

    

    void add_random_creatures(World& world, int k) {
        std::vector<Vec2> coords = TerrainQuery::random_unoccupied_coords(world.entity_map, world.territory, k);
        
        for (size_t i = 0; i < coords.size(); i++) {
            Entity random_creature = CreatureFactory::gen_creature(CreatureConfig{.position = coords[i]});
            WorldMotor::add_entity(world, std::move(random_creature));
        }
    }

    void add_specie_random_creature(World& world, int k, CreatureSpecies specie) {
        std::vector<Vec2> coords = TerrainQuery::random_unoccupied_coords(world.entity_map, world.territory, k);

        for (size_t i = 0; i < coords.size(); i++) {
            Entity random_creature = CreatureFactory::gen_creature(CreatureConfig{.position = coords[i], .specie = specie});
            WorldMotor::add_entity(world, std::move(random_creature));
        }
    }

    void delete_entity(World& world, Id id) {
        Vec2 position = EntityGetters::get_position(world.entities.at(id));
        world.entity_map.del(position);
        world.entities.del(id);
    }

} // namespace WorldMotor

namespace WorldFactory {

    World create_world(PresetWorld preset) {
        World world{
            .territory = TerrainFactory::gen_terrain(
                Vec2{preset.size.x, preset.size.y},
                preset.scale,
                preset.seed
            ),
            .entity_map = EntityMap{},
            .entities = EntitiesRegistry{},
            .reproductive_buffer = ReproductiveBuffer{},
            .log = Log{}
        };

        return world;
    }

    World create_crab_chaos() {
        PresetWorld preset{.seed = TerrainFactory::gen_seed(), .size = Vec2{7, 7}, .scale=ScaleGenValues::LONG};

        auto world = WorldFactory::create_world(preset);

        WorldMotor::add_specie_random_creature(world, 10, CreatureSpecies::CRAB);
        return world;
    }

    World create_paranoic() {
        PresetWorld preset{.seed = TerrainFactory::gen_seed(), .size = Vec2{5, 5}, .scale=ScaleGenValues::LONG_LONG};
        auto world = WorldFactory::create_world(preset);

        WorldMotor::add_specie_random_creature(world, 5, CreatureSpecies::HIPPOPOTAMUS);
        return world;
    }

    World create_titanic() {
        PresetWorld preset{.seed=TerrainFactory::gen_seed(), .size=Vec2(10, 11), .scale=ScaleGenValues::MEDIUM};
        auto world = WorldFactory::create_world(preset);
        WorldMotor::add_random_creatures(world, 10);
        return world;
    }

    World create_normal() {
        PresetWorld preset{.seed=TerrainFactory::gen_seed(), .size=Vec2{9, 9}, .scale=ScaleGenValues::MEDIUM};
        auto world = WorldFactory::create_world(preset);

        WorldMotor::add_random_creatures(world, 10);
        return world;
    }

} // namespace WorldFactory