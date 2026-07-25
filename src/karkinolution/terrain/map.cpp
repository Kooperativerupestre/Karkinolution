#include <FastNoiseLite.h>
#include <karkinolution/terrain/map.hpp>
#include <karkinolution/utils/k_random.hpp>
#include <format>
#include <karkinolution/core/error.hpp>
#include <karkinolution/terrain/soil.hpp>
#include <algorithm>

namespace TerrainQuery {

    std::vector<Vec2> all_unoccupied_coords(const EntityMap& entity_map, const Territory& territory) {
        std::vector<Vec2> coords{};

        for (const auto& c : territory.keys()) {
            if (!entity_map.exists(c)) {
                coords.push_back(c);
            }
        }
        return coords;
    }

    std::vector<Vec2> random_unoccupied_coords(const EntityMap& entity_map, const Territory& territory, int n) {
        std::vector<Vec2> coords = TerrainQuery::all_unoccupied_coords(entity_map, territory);

        if (n <= 0 || coords.empty()) {
            return {};
        }

        const size_t requested = static_cast<size_t>(std::min(n, static_cast<int>(coords.size())));
        std::shuffle(coords.begin(), coords.end(), gen);
        coords.erase(coords.begin() + requested, coords.end());
        return coords;
    }

} 

// TerrainMotor
namespace TerrainMotor {

    void add_entity(Id id, Vec2 position, const Territory& territory, EntityMap& entity_map) {
        if (!territory.exists(position)) {
            throw CoordinateNotFoundError(std::format(
    "Failed to execute terrain operation: coordinate ({}, {}) was not found in the territory.", 
    position.x, position.y
));
        }
        entity_map.add(position, id);
    }

    void delete_coord(Vec2 coord, Territory&territory, EntityMap& entity_map) {
        if (!territory.exists(coord)) {
            throw CoordinateNotFoundError(std::format("Coordinate ({}, {}) was not found", coord.x, coord.y));
        }
        territory.del(coord);
        entity_map.del(coord);
    }

    void move(Vec2 old_coord, Vec2 new_coord, EntityMap& entity_map, Territory& territory) {
        if (!territory.exists(new_coord)) {
            throw CoordinateNotFoundError(std::format("Coordinate ({}, {}) was not found", new_coord.x, new_coord.y));
        }
        Id id = entity_map.at(old_coord);
        entity_map.del(old_coord);
        entity_map.add(new_coord, id);
    }
}
namespace Geometry {
    std::unordered_map<Vec2, BlockData> neighbors_x_y(Vec2 position, const Territory& territory, const EntityMap& entity_map, Vec2 radius, bool include_self) {
        std::unordered_map<Vec2, BlockData> neighbors{};
        int x_r = radius.x;
        int y_r = radius.y;
        
        for (int row = -y_r; row < y_r + 1; row++) {
            for (int column = -x_r; column < x_r + 1; column++) {
                if (row == 0 && column == 0 && include_self) {
                    const SoilPiece* piece = &territory.at(position);
                    neighbors[position] = BlockData(piece, std::nullopt); 
                    continue;
                }

                Vec2 coord_moved = Vec2(column + position.x, row + position.y);

                if (territory.exists(coord_moved)) {
                    const Id* id = entity_map.try_at(coord_moved);
                    const SoilPiece* piece = &territory.at(coord_moved);
                    std::optional<Id> creature_id;

                    if (id) {
                        creature_id = *id;
                    }
                    neighbors[coord_moved] = BlockData{piece, creature_id};
                }
            }
        }
        return neighbors;
    }   

}
// 
namespace TerrainFactory {

    int gen_seed() {
        return std::uniform_int_distribution<int>(1, 1000)(gen);
    }


    SoilTypes value_to_cell_type(float value) {
        if (value <= -0.3f) {
            return SoilTypes::WATER;
        }
        else if (value <= -0.1f) {
            return SoilTypes::SAND;
        }
        else if (value <= 0.7f) {
            return SoilTypes::DIRT;
        }
        else {
            return SoilTypes::ROCK;
        }
    }
    Territory gen_terrain(Vec2 radius, float scale, int seed) {
        Territory territory{static_cast<uint16_t>(radius.x), static_cast<uint16_t>(radius.y)};
        FastNoiseLite noise;
        noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
        noise.SetSeed(seed);                                

        for (int x = 0; x < radius.x; ++x) {
            for (int y = 0; y < radius.y; ++y) {
                
                float value = noise.GetNoise(x * scale, y * scale);
                
                SoilTypes terrain_type = TerrainFactory::value_to_cell_type(value);
                
                auto cell = SoilF::gen_soil_piece(terrain_type);
                territory.add(Vec2{x, y}, std::move(cell));
            }
        }
        return territory; 
    }
}