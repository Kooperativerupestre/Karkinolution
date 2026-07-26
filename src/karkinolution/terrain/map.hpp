#pragma once
#include <vector>
#include <cstdint>
#include <karkinolution/core/basestorage.hpp>
#include <karkinolution/terrain/soil.hpp>
#include <karkinolution/core/vec2.hpp>
#include <karkinolution/organism/ontology.hpp>
#include <optional>

/*
 * Copyright 2026 Koöperative Rüpestrën
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 */

struct ScaleGenValues {
    static constexpr double LONG_LONG = 0.05;
    static constexpr double LONG = 0.15;
    static constexpr double MEDIUM    = 0.30;
    static constexpr double FRAGMENTED = 1.0;
};

class Territory : public BaseStorage<Vec2, SoilPiece> {
    private:

    uint16_t size_x;
    uint16_t size_y;

    public:

    Territory(uint16_t size_x, uint16_t size_y) : BaseStorage<Vec2, SoilPiece>(),
    size_y(size_y) {}
};

class EntityMap : public BaseStorage<Vec2, Id> {
    public:
    EntityMap() : BaseStorage<Vec2, Id>() {}
};



struct BlockData {
    const SoilPiece* soil;
    std::optional<Id> id;
};

namespace TerrainQuery {
    std::vector<Vec2> all_unoccupied_coords(const EntityMap& entity_map, const Territory&territory);
    std::vector<Vec2> random_unoccupied_coords(const EntityMap& entity_map, const Territory&territory, int n = 1);
};

namespace TerrainMotor {
    void add_entity(Id id, Vec2 position, const Territory&territory, EntityMap&entity_map);
    void delete_coord(Vec2 coord, Territory&territory, EntityMap&entity_map);
    void move(Vec2 old_coord, Vec2 new_coord, EntityMap&entity_map, Territory&territory);
};

namespace TerrainFactory {
    int gen_seed();
    SoilTypes value_to_soil_type(float value);
    Territory gen_terrain(Vec2 radius, float scale, int seed);
};;
